#pragma once

#include "PortAudioCommon.h"
#include <AlternativeAudio\Device\OAudioDevice.h>

namespace PortAudio {
	class PortAudioDevice : public AlternativeAudio::OAudioDevice {
	public:
		AZ_RTTI(PortAudioDevice, "{9CAB599F-6A78-4389-B4B9-C775F1EC64A8}", AlternativeAudio::OAudioDevice);
	public:
		PortAudioDevice();
		PortAudioDevice(long long deviceIndex, double samplerate, AlternativeAudio::AudioFrame::Type audioFormat, void * userdata);
		~PortAudioDevice();
	public:
		void SetStream(double samplerate, AlternativeAudio::AudioFrame::Type audioFormat, void * userdata);
		void SetResampleQuality(AlternativeAudio::AAResampleQuality quality);
		AlternativeAudio::OAudioDeviceInfo GetDeviceInfo() { return this->m_info; }
	public:
		long long PlaySource(AlternativeAudio::IAudioSource * source);
		void PauseSource(long long id);
		void ResumeSource(long long id);
		void StopSource(long long id);
		bool IsPlaying(long long id);
		AlternativeAudio::AudioSourceTime GetTime(long long id);
		void SetTime(long long id, double time);
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<PortAudioDevice, OAudioDevice>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
		}
	private:
		AlternativeAudio::OAudioDeviceInfo m_info;
	private:
		struct PlayingAudioSource {
			AlternativeAudio::IAudioSource* audioSource{ nullptr };
			long long currentFrame{ 0 };
			long long startFrame{ 0 };
			long long endFrame{ 0 };
			bool loop{ false };
			bool paused{ false };
			float vol{ 1.0f };
		};
	private:
		typedef AZStd::unordered_map<long long, PlayingAudioSource *> PlayingAudioSourcesMap;
		PlayingAudioSourcesMap m_playingAudioSource;

		long long m_nextPlayID;

		std::vector<long long> m_stoppedAudioFiles;
	private: //audio stream
		PaStream *m_pAudioStream;
		AlternativeAudio::AudioFrame::Type m_audioFormat;
		long long m_device;
		void * m_hostApiSpecificStreamInfo;
	private: //samplerate conversion
		double m_sampleRate;
		SRC_STATE * m_pSrcState;
		AlternativeAudio::AAResampleQuality m_rsQuality;
	private: //mutexes
		AZStd::mutex m_callbackMutex, m_errorMutex;
	private:
		static AlternativeAudio::AudioFrame::Frame * CreateBuffer(AlternativeAudio::AudioFrame::Type type, long long length);
		static int getNumberOfChannels(AlternativeAudio::AudioFrame::Type type);
	protected:
		int paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
	public:
		static int paCallbackCommon(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
	};
}