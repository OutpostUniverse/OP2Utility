#pragma once

#include "Streams/SeekableReader.h"
#include <gtest/gtest.h>

template <class T>
Stream::SeekableReader* CreateSeekableReader();