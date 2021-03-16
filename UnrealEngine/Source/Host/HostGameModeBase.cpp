#include "HostGameModeBase.h"
#include "third_include_start.h"

#include <boost/date_time.hpp>
#include <fmt/format.h>

#include "third_include_end.h"


void AHostGameModeBase::StartPlay()
{
    Super::StartPlay();
    if(GEngine != nullptr)
        GEngine->AddOnScreenDebugMessage(
            -1,
            5.0f,
            FColor::Yellow,
            fmt::format(
                "{} Hello World",
                to_iso_extended_string(boost::posix_time::microsec_clock::local_time())
            ).c_str()
        );
}
