#define DAR_MODULE_NAME "Audio"

#include "Audio.hpp"

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include <objbase.h>

#include <DarEngine.hpp>

namespace Audio
{
  FMOD::Studio::System* studioSystem = nullptr;
  FMOD::System* coreSystem = nullptr;
  constexpr int sampleRate = 48000;
  int driversCount = 0;

#define initializeErrorCheck(call) \
  { \
    FMOD_RESULT result = call; \
    if(result != FMOD_OK) { \
      logError(#call " failed: %d - %s", result, FMOD_ErrorString(result)); \
      return false; \
    } \
  }

  bool initialize()
  {
    // from doc: Before calling any FMOD functions it is important to ensure COM is initialized. 
    // You can achieve this by calling CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED) 
    // on each thread that will interact with the FMOD API. This is balanced with a call to 
    // CoUninitialize() when you are completely finished with all calls to FMOD.
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    initializeErrorCheck(FMOD::Studio::System::create(&studioSystem));

    initializeErrorCheck(studioSystem->getCoreSystem(&coreSystem));
    initializeErrorCheck(coreSystem->setSoftwareFormat(sampleRate, FMOD_SPEAKERMODE_DEFAULT, 0));

    // from doc: To ensure correct behavior FMOD assumes when using the WASAPI output mode 
    // (default for Windows Vista and newer) that you call System::getNumDrivers, 
    // System::getDriverInfo and System::init from your UI thread. This ensures that any platform 
    // specific dialogs that need to be presented can do so. This recommendation comes from the 
    // IAudioClient interface docs on MSDN which state: In Windows 8, the first use of IAudioClient 
    // to access the audio device should be on the STA thread. Calls from an MTA thread may result 
    // in undefined behavior.
    initializeErrorCheck(coreSystem->getNumDrivers(&driversCount));
    initializeErrorCheck(coreSystem->getDriverInfo());

    // 1024 value taken from example, I don't know what value should be put here
    // also initializes coreSystem
    initializeErrorCheck(studioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));
  }

  void update(const GameState& gameState)
  {
  
  }

  void unitialize()
  {
    coreSystem->release();

    studioSystem->release();
    studioSystem = nullptr;

    CoUninitialize();
  }
} // namespace Audio