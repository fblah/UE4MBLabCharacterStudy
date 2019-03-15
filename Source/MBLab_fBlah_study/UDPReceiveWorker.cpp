#include "UDPReceiveWorker.h"
#include "MBLab_fBlah_study.h"


UDPReceiveWorker* UDPReceiveWorker::Runnable = NULL;

UDPReceiveWorker::UDPReceiveWorker(TArray<uint8>& ReceivedData, FSocket* InSocket, FTimespan& InWaitTime)
	: Socket(InSocket)
	, StopTaskCounter(0)
	, WaitTime(InWaitTime)
	, Stopping(false)
{

	const bool bAutoDeleteSelf = false;
	const bool bAutoDeleteRunnable = false;

	// Link the data
	ReceivedDataRef = &ReceivedData;

	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	Thread = FRunnableThread::Create(this, TEXT("UDPReceiveWorker"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more

}

UDPReceiveWorker::~UDPReceiveWorker()
{
	delete Thread;
	Thread = NULL;
	SocketSubsystem = nullptr;
	Socket = nullptr;
	ReceivedDataRef = nullptr;
	//myNetworkingWrapper = nullptr;
}

// Init
bool UDPReceiveWorker::Init()
{
	return true;
}

//Run
uint32 UDPReceiveWorker::Run()
{
	TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();

	while (!Stopping)
	{
		if (!Socket->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
		{
			continue;
		}

		uint32 Size;

		while (Socket->HasPendingData(Size))
		{

			int32 Read = 0;
			Socket->RecvFrom(ReceivedDataRef->GetData(), ReceivedDataRef->Num(), Read, *Sender);

		}

	}



	return 0;
}


// Stop
void UDPReceiveWorker::Stop()
{
	StopTaskCounter.Increment();
}

UDPReceiveWorker* UDPReceiveWorker::JoyInit(TArray<uint8>& ReceivedData, FSocket* InSocket, FTimespan& InWaitTime)
{
	//Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new UDPReceiveWorker(ReceivedData, InSocket, InWaitTime);
	}
	return Runnable;
}

void UDPReceiveWorker::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

void UDPReceiveWorker::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}

//
//bool UDPReceiveWorker::IsThreadFinished()
//{
//	if (Runnable) return Runnable->IsFinished();
//	return true;
//}
