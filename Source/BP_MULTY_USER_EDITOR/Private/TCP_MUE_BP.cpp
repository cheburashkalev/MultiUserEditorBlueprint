// Fill out your copyright notice in the Description page of Project Settings.


#include "TCP_MUE_BP.h"


#include "Async/Async.h"

UTCP_MUE_BP::UTCP_MUE_BP(const FObjectInitializer& ObjectInitializer)
{
		bShutDown = false;
		bConnecting = false;
}
void UTCP_MUE_BP::BeginDestroy()
{
	if(!bConnecting)
		Close();
	bShutDown = true;
	Super::BeginDestroy();
	
}
void UTCP_MUE_BP::Close()
{
	
	if (Socket)
	{
		for (auto RecThreald : RecThreads)
		{
			RecThreald->Stop();
		}
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
	if (RecSocket)
	{
		RecSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(RecSocket);
		RecSocket = nullptr;
	}
	GEditor->GetTimerManager()->ClearTimer(ConnectCheckHandler);
}
void UTCP_MUE_BP::SendData(FString Message)
{
	for (auto SocketThread : RecThreads)
	{
		SocketThread->SendData(Message);
	}
}
void UTCP_MUE_BP::ConnectTickCheck()
{
	
	//FPlatformProcess::Sleep(1.0f);
if(!Socket)
{
	UE_LOG(LogTemp, Warning, TEXT("ERROR SOCKET"));
}
	bool bPending = false;
	if (Socket->HasPendingConnection(bPending) && bPending)
	{
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		RecSocket = Socket->Accept(*RemoteAddress, TEXT("Receive Socket"));
		UTCP_MUE_RSThread* RSThread = NewObject<UTCP_MUE_RSThread>(this);
		RecThreads.Add(RSThread);
		RSThread->ReceiveSocketDataDelegate = ReceiveSocketDataDelegate;
		RSThread->LostConnectionDelegate.AddDynamic(this, &UTCP_MUE_BP::OnDisConnected);
		RSThread->StartThread(RecSocket, SendDataSize, RecDataDize);
		ClientConnectDelegate.Broadcast(RemoteAddress->ToString(false), RemoteAddress->GetPort());
	}
	if (!ReceiveSocketDataDelegate.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT(" ReceiveSocketDataDelegate没有绑定 "));
	}
}

bool UTCP_MUE_BP::CreateServer(const FString& IP, int32 Port, int32 ReceiveSize, int32 SendSize)
{
	this->SendDataSize = SendSize;
	this->RecDataDize = ReceiveSize;
		FIPv4Address ServerAddr;
		if (!FIPv4Address::Parse(IP, ServerAddr))
		{
			UE_LOG(LogTemp, Error, TEXT("Server Ip %s is illegal"), *IP);
		}
		Socket = FTcpSocketBuilder(TEXT("Socket Listener"))
			.AsReusable()
			.AsBlocking()
			.BoundToAddress(ServerAddr)
			.BoundToPort(Port)
			.Listening(8)
			.WithReceiveBufferSize(SendDataSize)
			.WithSendBufferSize(RecDataDize);
	
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("Create  Socket Error!"));
		return false;
	}
	else
	{

		GEditor->GetTimerManager()->SetTimer(ConnectCheckHandler, this, &UTCP_MUE_BP::ConnectTickCheck, 1, true);


		return true;
	}
}

void UTCP_MUE_BP::ConnectServer(const FString& IP, int32 Port)
{

	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [&]()
		  {
			FIPv4Endpoint ServerEndpoint;
			FIPv4Endpoint::Parse(IP, ServerEndpoint);
			TSharedPtr<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			bool Success = true;
			addr->SetIp(*IP, Success);
			if (!Success)
			{
				ConnectedResultDelegate.Broadcast(false);
				return;
			}
			addr->SetPort(Port);

			if (!bShutDown && Socket->Connect(*addr))
			{
				UTCP_MUE_RSThread* RSThread = NewObject<UTCP_MUE_RSThread>();
				RecThreads.Add(RSThread);
				RSThread->ReceiveSocketDataDelegate = ReceiveSocketDataDelegate;
				RSThread->LostConnectionDelegate.AddDynamic(this, &UTCP_MUE_BP::OnDisConnected);
				RSThread->StartThread(Socket, SendDataSize, RecDataDize);
				UE_LOG(LogTemp, Warning, TEXT("Client Connect Success"));
				if (ConnectedResultDelegate.IsBound())
				{
					ConnectedResultDelegate.Broadcast(true);
				}
			    
			}
			else
			{
				ESocketErrors LastErr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();

				UE_LOG(LogTemp, Warning, TEXT("Connect failed with error code (%d) error (%s)"), LastErr, ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetSocketError(LastErr));
				if (!bShutDown && ConnectedResultDelegate.IsBound())
				{
					ConnectedResultDelegate.Broadcast(false);
				}
			}
		 // bConnecting = false;
			return;
		  });
	
}

bool UTCP_MUE_BP::CreateClient(const FString& IP, int32 Port, int32 ReceiveSize, int32 SendSize)
{
	Socket = FTcpSocketBuilder(TEXT("Client Socket"))
	.AsReusable()
	.AsBlocking()
	.WithReceiveBufferSize(ReceiveSize)
	.WithSendBufferSize(SendSize);
	ConnectServer(IP, Port);
	return Socket ? true:false;
	
}
void UTCP_MUE_BP::OnDisConnected(UTCP_MUE_RSThread* pThread)
{
	UE_LOG(LogTemp, Warning, TEXT("Client lost"));
	RecThreads.Remove(pThread);
	if (ConnectedResultDelegate.IsBound())
	{
		ConnectedResultDelegate.Broadcast(false);
	}
}
//////////////////////////////////////////////////////////////////
///client object
void UTCP_MUE_RSThread::BeginDestroy()
{
	Stop();
	Super::BeginDestroy();
}
uint32 UTCP_MUE_RSThread::Run()
{
	while (!bThreadStop && ConnectSocket)
	{
		uint32 Size;
		bool LostConnect = false;
		ConnectSocket->HasPendingConnection(LostConnect);
		ConnectSocket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan(0, 0, 5));
		if (LostConnect && !ConnectSocket->HasPendingData(Size))
		{
			ReceiveData.Init(0, 100);
			int32 Temp;
			if (!ConnectSocket->Recv(ReceiveData.GetData(), 0, Temp))
			{
				UE_LOG(LogTemp, Warning, TEXT(" Connect lost "));
				Stop();
				continue;
			}

		}


		if (ConnectSocket && ConnectSocket->HasPendingData(Size))
		{
			int32 minSize = FMath::Min(Size, MaxRecDataSize);
			ReceiveData.Init(0, minSize + 1);
			int32 Readed;
			if (!ConnectSocket->Recv(ReceiveData.GetData(), minSize, Readed))
			{
				UE_LOG(LogTemp, Warning, TEXT(" Connect lost "));
				Stop();
				continue;
			}
			FString ReceivedStr= FString(UTF8_TO_TCHAR(ReceiveData.GetData()));
			UE_LOG(LogTemp, Warning, TEXT("receive message = %s  size = %d"), *ReceivedStr, minSize);
			
			AsyncTask(ENamedThreads::GameThread, [&,ReceivedStr]()
			{
				if (ReceiveSocketDataDelegate.IsBound())
				{
					//FString ReceivedStr = FString(UTF8_TO_TCHAR(ReceiveData.GetData()));
					ReceiveSocketDataDelegate.Broadcast(ReceivedStr);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT(" thread ReceiveSocketDataDelegate num is 0 "));
				}
			});
		}
			


	}
	ReceiveData.Empty();
	
	return 0;
}
void UTCP_MUE_RSThread::StartThread(FSocket* Socket, uint32 SizeSend, uint32 SizeRec)
{
	this->ConnectSocket = Socket;
	this->MaxSendDataSize = SizeSend;
	this->MaxRecDataSize = SizeRec;
	FRunnableThread::Create(this, TEXT("Receive Threald"));
}

void UTCP_MUE_RSThread::Stop()
{
	if (LostConnectionDelegate.IsBound())
	{
		LostConnectionDelegate.Broadcast(this);
	}
	
	bThreadStop = true;
	ConnectSocket = nullptr;
}

void UTCP_MUE_RSThread::SendData(FString Message)
{

	std::string strSend(TCHAR_TO_UTF8(*Message));
	SendDataBuffer.Init(0, strSend.size());
	memcpy(SendDataBuffer.GetData(), strSend.data(), strSend.size());
	int32 sent = 0;
	if (SendDataBuffer.Num() >= (int32)MaxSendDataSize)
	{
		UE_LOG(LogTemp, Error, TEXT("Send Data Size is Larger than Max Size for set"));
	}
	else
	{
		if (ConnectSocket && ConnectSocket->Send(SendDataBuffer.GetData(), SendDataBuffer.Num(), sent))
		{
			UE_LOG(LogTemp, Warning, TEXT("___Send Succeed! msg = %s messageSize = %d sended = %d"), *Message, SendDataBuffer.Num(), sent);

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("___Send Failed!"));
		}
	}


}


