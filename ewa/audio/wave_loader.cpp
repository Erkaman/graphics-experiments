#include "wave_loader.hpp"

#include "ewa/resource_manager.hpp"
#include "ewa/log.hpp"
#include "ewa/file.hpp"

#include "openal.hpp"


constexpr int RIFF = 0x52494646;
constexpr int WAVE = 0x57415645;
constexpr int FMT = 0x666d7420;
constexpr int DATA_UPPER = 0x64617461; // 'DATA'
constexpr int DATA_LOWER = 0x64617461; // 'data'


int ReadInt(File& file) {
    char *bytes = (char *)file.ReadArray(4);

    return
	bytes[0] << 24 |
	bytes[1] << 16 |
	bytes[2] << 8 |
	bytes[3] << 0;
}

int ReadLittleEndianInt(File& file) {
    return  *((int *)file.ReadArray(4));
}

int ReadLittleEndianShort(File& file) {
    return  *((short *)file.ReadArray(2));
}


WaveData WaveLoader::Load(const std::string& filename) {

    std::string resourcePath = ResourceManager::GetInstance().FindResource(filename);

    LOG_I("found file: %s", resourcePath.c_str());

    File file(resourcePath, FileModeReading);

    const int chunkID = ReadInt(file);
    if(chunkID != RIFF) {
	LOG_E("WAVE file has an invalid chunk ID: %d", chunkID);
    }

    const int chunkSize = ReadLittleEndianInt(file);

    const int format = ReadInt(file);

    if(format != WAVE) {
	LOG_E("WAVE file has an invalid format: %d", chunkID);
    }

    int readBytes = 4;

    short audioFormat = 0;
    short numChannels= 0;
    int sampleRate = 0;
    int byteRate = 0;
    short blockAlign = 0;
    short bitsPerSample = 0;

    void* audioData = NULL;

    bool readFmtChunk = false;


    WaveData waveData;



    while(readBytes < chunkSize) {

	const int subchunkID = ReadInt(file);

			const int subchunkSize = ReadLittleEndianInt(file);

			// word align the chunk size.
			int numsubchunkBytes = (subchunkSize%2 == 1) ? subchunkSize+1 : subchunkSize;

			readBytes += 8 + numsubchunkBytes;

			if(subchunkID == FMT) {
				readFmtChunk = true;

				audioFormat = ReadLittleEndianShort(file);

				if(audioFormat != 1) {

				    LOG_E("Unsupported audio format %d. Only PCM is supported", audioFormat);
				}

				numChannels = ReadLittleEndianShort(file);

				if(numChannels != 1 && numChannels != 2) {
				    LOG_E("Unsupported number of channels %d", numChannels);
				}

				sampleRate = ReadLittleEndianInt(file);

				byteRate = ReadLittleEndianInt(file);
				blockAlign = ReadLittleEndianShort(file);
				bitsPerSample = ReadLittleEndianShort(file);

				if(bitsPerSample != 8 && bitsPerSample != 16) {
				    LOG_E("Unsupported BitsPerSample %d", sampleRate);
				}

			} else if(subchunkID == DATA_UPPER ||subchunkID == DATA_LOWER) {
				if(!readFmtChunk) {
					LOG_E("Invalid WAVE file. The DATA chunk cannot precede the FMT chunk");
				}

				void* buffer =malloc(numsubchunkBytes);

				buffer = file.ReadArray(numsubchunkBytes);


				// TOOD: conevrt here.

/*
				if(bitsPerSample == 16) {
				    LOG_E("16 bits per sample not yet supported!");
				}*/

				audioData = buffer;
				waveData.audioDataSize = numsubchunkBytes;

/*				char* bs = (char *)audioData;

				for(int i = 0; i < 10; ++i) {
				    LOG_I("b: %d", bs[i]);
				    }*/

				//audioData =convertAudioDataToLittleEndian(buffer, sampleRate == 16);

			} else {

				// unknown chunk. skip it.
			    LOG_I("skipped chunk: %d", subchunkID);
			    file.Skip(numsubchunkBytes);

			}
		}

		if(audioData == NULL) {
			LOG_E("Invalid WAVE file: no DATA chunk could be found");
		}

		if(!readFmtChunk) {
			LOG_E("Invalid WAVE file: no FMT chunk could be found");
		}


		int channels  =0;


		if (numChannels == 1) {
			if (bitsPerSample == 8) {
				channels = AL_FORMAT_MONO8;
			} else if (bitsPerSample == 16) {
				channels = AL_FORMAT_MONO16;
			}
		} else if (numChannels == 2) {
			if (bitsPerSample == 8) {
				channels = AL_FORMAT_STEREO8;
			} else if (bitsPerSample == 16) {
				channels = AL_FORMAT_STEREO16;
			}
		}



		LOG_I("sample rate: %d", sampleRate);
		LOG_I("channels: %d", channels);


		waveData.audioData = audioData;
		waveData.sampleRate = sampleRate;
		waveData.channels = channels;

		return waveData;

}

/*
0x01, 0x00, 0x01, 0x00,  0xfd, 0xff

 */
