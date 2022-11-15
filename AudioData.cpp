#include "AudioData.h"
#include <math.h>

AudioData::AudioData(unsigned nframes, unsigned R, unsigned nchannels) :
  frame_count(nframes), sampling_rate(R), channel_count(nchannels)
{
  fdata.resize(nframes * nchannels);
}

float AudioData::sample(unsigned frame, unsigned channel) const
{
  return fdata[(2 * frame) + channel];
}

float & AudioData::sample(unsigned frame, unsigned channel)
{
  return fdata[(2 * frame) + channel];
}

static float calculateDCOffset(AudioData &ad, int channel)
{
  float dcOffset = 0;

  for (unsigned i = 0; i < ad.frames(); i++)
  {
    dcOffset += ad.data()[(i * ad.channels()) + channel];
  }
  dcOffset /= ad.frames();

  return dcOffset;
}

static float calculateLargestSample(AudioData &ad)
{
  float largestSample = 0;

  for (unsigned i = 0; i < ad.frames(); i++)
  {
    for (unsigned j = 0; j < ad.channels(); j++)
    {
      if (abs(ad.data()[(ad.channels() * i) + j]) > largestSample)
      {
        largestSample = abs(ad.data()[(ad.channels() * i) + j]);
      }
    }
  }
  return largestSample;
}

static void removeDCOffset(AudioData &ad, float offset, int channel)
{
  for (unsigned i = 0; i < ad.frames(); i++)
  {
    ad.data()[(ad.channels() * i) + channel] -= offset;
  }
}

static void changeAmplitude(AudioData &ad, float dB, float largestSample)
{
  float ceiling = 1.0f * pow(10, dB / 20);
  ceiling /= largestSample;

  for (unsigned i = 0; i < ad.frames(); i++)
  {
    for (unsigned j = 0; j < ad.channels(); j++)
    {
      ad.data()[(ad.channels() * i) + j] *= ceiling;
    }
  }
}

void normalize(AudioData & ad, float dB)
{
  std::vector<float> dcOffsets;
  float largestSample;

  dcOffsets.resize(ad.channels());

  for (int i = 0; i < ad.channels(); i++)
  {
    dcOffsets[i] = calculateDCOffset(ad, i);
  }

  for (int i = 0; i < ad.channels(); i++)
  {
    removeDCOffset(ad, dcOffsets[i], i);
  }

  largestSample = calculateLargestSample(ad);
  changeAmplitude(ad, dB, largestSample);
}