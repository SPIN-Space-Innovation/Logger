/**
 * MIT License
 * 
 * Copyright (c) 2022 SPIN - Space Innovation
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#ifndef __LOGGER__LOGGER__H__
#define __LOGGER__LOGGER__H__ (0 * 10000 + 2 * 100 + 0)

#include <stdarg.h>
#include <stdint.h>

#ifdef ARDUINO
    #include <Arduino.h>
    #include <SD.h>
#else
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <iostream>
#endif

#include <SPIN/Log/LogLevel.hpp>

#include <SPIN/Log/Sinks/ISink.hpp>
#include <SPIN/Log/Sinks/FileSink.hpp>
#include <SPIN/Log/Sinks/SerialSink.hpp>
#include <SPIN/Log/ILogger.hpp>
#include <SPIN/Log/CFormattedLogger.hpp>

#endif/*!__LOGGER__LOGGER__H__*/