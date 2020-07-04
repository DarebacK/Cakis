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

  // 1024 value taken from example, I don't know what value should be put here
  // also initializes coreSystem
  initializeErrorCheck(studioSystem->initialize(
    1024/*maxChannels*/, 
    FMOD_STUDIO_INIT_NORMAL, 
    FMOD_INIT_NORMAL, 
    nullptr/*extraDriverData*/
  ));

  return true;
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