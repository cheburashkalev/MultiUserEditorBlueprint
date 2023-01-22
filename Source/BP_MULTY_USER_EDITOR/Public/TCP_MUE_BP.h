// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "HAL/Runnable.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/NoExportTypes.h"
#include "TCP_MUE_BP.generated.h"
class FRunableThread;
class UTCP_MUE_RSThread;
/**
 * 
 */
DECLARE_DELEGATE_TwoParams(FClientConnectDelegate, FString, int32);
DECLARE_DELEGATE_OneParam(FConnectedResultDelegate, bool);
DECLARE_DELEGATE_OneParam(FReceiveSocketDataDelegate, FString); //Receive Connect Callback
DECLARE_DELEGATE_OneParam(FLostConnectionDelegate, UTCP_MUE_RSThread*);	//DisConnect Callback

UCLASS()
class BP_MULTY_USER_EDITOR_API UTCP_MUE_RSThread : public UObject, public FRunnable
{
	GENERATED_BODY()
public:
	virtual void BeginDestroy() override;
	//FRunnable Interface
	virtual bool Init() override { return true; }
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override {}
	//End Interface
	

	void StartThread(FSocket* Socket, uint32 SizeSend, uint32 SizeRec);
	void SendData(FString Data);

public:
	FReceiveSocketDataDelegate ReceiveSocketDataDelegate;
	FLostConnectionDelegate	LostConnectionDelegate;

protected:
	FSocket* ConnectSocket;
	
	uint32 MaxSendDataSize;
	uint32 MaxRecDataSize;
	TArray<uint8> ReceiveData;
	TArray<uint8> SendDataBuffer;
	FRunnableThread* pThread;
	bool bThreadStop;
};
UCLASS()
class BP_MULTY_USER_EDITOR_API UTCP_MUE_BP : public UObject
{
	GENERATED_BODY()
public:
	UTCP_MUE_BP(const FObjectInitializer& ObjectInitializer);
	virtual void BeginDestroy() override;
	void ConnectTickCheck();
	UFUNCTION(BlueprintCallable, Category = Network)
		bool CreateServer(const FString& IP, int32 Port, int32 ReceiveSize = 1024, int32 SendSize = 1024);

	void ConnectServer(const FString& IP, int32 Port);
	UFUNCTION(BlueprintCallable, Category = Network)
	bool CreateClient(const FString& IP, int32 Port, int32 ReceiveSize = 1024, int32 SendSize = 1024);

	UFUNCTION(BlueprintCallable, Category = Network)
		void SendData(FString Message);

	UFUNCTION(BlueprintCallable, Category = Network)
		void Close();
	
	UFUNCTION()
	    void OnDisConnected(UTCP_MUE_RSThread* pThread);
	UPROPERTY()
	TArray<class UTCP_MUE_RSThread*> RecThreads;
	class FSocket* Socket;
	FSocket* RecSocket;
	FString ServerIP;
	//UTCP_EditorTimer* t;
	int32 ServerPort;
	bool bShutDown;
	bool bConnecting;
	int32 SendDataSize;
	int32 RecDataDize;
	FTimerHandle ConnectCheckHandler;
public:
	//UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = Network)
	FReceiveSocketDataDelegate ReceiveSocketDataDelegate;

	//UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = Network)
	FConnectedResultDelegate ConnectedResultDelegate;
	
	//UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = Network)
	FClientConnectDelegate ClientConnectDelegate;
	
};

