#include "HostGameModeBase.h"

void AHostGameModeBase::StartPlay()
{
    Super::StartPlay();
    if(GEngine != nullptr)
        GEngine->AddOnScreenDebugMessage(
            -1,
            5.0f,
            FColor::Yellow,
            fmt::format(
                u8"{} Hello World",
                to_iso_extended_string(boost::posix_time::microsec_clock::local_time())
            ).c_str()
        );
}
