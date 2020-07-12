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
FMOD::Studio::EventInstance* musicInstance = nullptr;
FMOD::Studio::Bus* masterBus = nullptr;
float masterVolume = 0.33f;

#define initializeErrorCheckFatal(call) \
result = call; \
if(result != FMOD_OK) { \
  logError(#call " failed: %d - %s", result, FMOD_ErrorString(result)); \
  return false; \
}

#define errorCheck(call) \
result = call; \
if(result != FMOD_OK) { \
  logError(#call " failed: %d - %s", result, FMOD_ErrorString(result)); \
} else

bool initialize()
{
  FMOD_RESULT result;

  // from doc: Before calling any FMOD functions it is important to ensure COM is initialized. 
  // You can achieve this by calling CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED) 
  // on each thread that will interact with the FMOD API. This is balanced with a call to 
  // CoUninitialize() when you are completely finished with all calls to FMOD.
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  initializeErrorCheckFatal(FMOD::Studio::System::create(&studioSystem));

  initializeErrorCheckFatal(studioSystem->getCoreSystem(&coreSystem));
  initializeErrorCheckFatal(coreSystem->setSoftwareFormat(sampleRate, FMOD_SPEAKERMODE_DEFAULT, 0));

  // 1024 value taken from example. I don't know what value should be put here.
  // Also initializes coreSystem.
  initializeErrorCheckFatal(studioSystem->initialize(
    1024/*maxChannels*/, 
    FMOD_STUDIO_INIT_NORMAL, 
    FMOD_INIT_NORMAL, 
    nullptr/*extraDriverData*/
  ));

  FMOD::Studio::Bank* masterBank = nullptr;
  errorCheck(studioSystem->loadBankFile("audio/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
  FMOD::Studio::Bank* stringsBank = nullptr;
  errorCheck(studioSystem->loadBankFile("audio/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));
  FMOD::Studio::Bank* musicBank = nullptr;
  errorCheck(studioSystem->loadBankFile("audio/music.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &musicBank));

  FMOD::Studio::EventDescription* musicDescription = nullptr;
  errorCheck(studioSystem->getEvent("event:/music/music", &musicDescription)) {
    errorCheck(musicDescription->createInstance(&musicInstance));
  }

  errorCheck(studioSystem->getBus("bus:/", &masterBus)) {
    masterBus->setVolume(masterVolume);
  }

  return true;
}

void update(const GameState& gameState)
{
  FMOD_RESULT result;

  if(musicInstance) {
    FMOD_STUDIO_PLAYBACK_STATE musicPlaybackState;
    errorCheck(musicInstance->getPlaybackState(&musicPlaybackState)) {
      if(musicPlaybackState != FMOD_STUDIO_PLAYBACK_PLAYING) {
        errorCheck(musicInstance->start());
      }
    }
  }

  errorCheck(studioSystem->update());
}
} // namespace Audio