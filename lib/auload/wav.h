//
//  wav.h
//  openaltest
//
//  Created by Mattias Holm on 2011-10-08.
//  Copyright 2011 Mattias Holm. All rights reserved.
//

#ifndef openaltest_wav_h
#define openaltest_wav_h

#include <OpenAL/al.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
  char ckID[4];
  uint32_t cksize;
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;

  uint16_t cbSize;
  uint16_t wValidBitsPerSample;
  uint32_t dwChannelMask;
  char SubFormat[16];
} wav_format_chunk_t;

typedef enum {
  WAVE_FORMAT_PCM = 0x0001,
  WAVE_FORMAT_IEEE_FLOAT = 0x0003,
  WAVE_FORMAT_ALAW = 0x0006,
  WAVE_FORMAT_MULAW = 0x0007,
  WAVE_FORMAT_EXTENSIBLE = 0xfffe,
} wav_format_t;

#define WAVE_CHUNK_FILE "RIFF"
#define WAVE_WAVE_ID "WAVE"
#define WAVE_CHUNK_FORMAT "fmt "
#define WAVE_CHUNK_FACT "fact"
#define WAVE_CHUNK_DATA "data"


typedef struct {
  char ckID[4];
  uint32_t cksize;
  uint32_t dwSampleLength;
} wav_fact_chunk_t;

typedef struct {
  char ckID[4];
  uint32_t cksize;
  char samples[];
} wav_data_chunk_t;

typedef struct {
  char ckID[4];
  uint32_t cksize;  
} wav_chunk_t;

typedef struct {
  char ckID[4];
  uint32_t cksize;
  char WAVEID[4];
} wav_header_t;

typedef struct { 
  wav_format_t format;
  uint32_t nSamplesPerSec;
  uint16_t bitsPerSample;
  uint16_t nChannels;

  size_t sampleByteSize;
  char *samples;
} wav_file_t;

wav_file_t* wav_open(const char *fileName);
void wav_dispose(wav_file_t *wav);
void wav_info(wav_file_t *wav);


ALuint wav_getbuffer(wav_file_t *wav);

#endif