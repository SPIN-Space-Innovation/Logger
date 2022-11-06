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

#if !defined(__LOGGER__SPIN__LOG__ILOGGER__H__) && defined(__cplusplus)
#define __LOGGER__SPIN__LOG__ILOGGER__H__

#ifdef ARDUINO
    #include <stdarg.h>
    #include <stddef.h>
    #include <stdlib.h>
    #include <string.h>
#else
    #include <cstdarg>
    #include <cstddef>
    #include <cstdlib>
    #include <cstring>
    #include <exception>
#endif

#include <SPIN/Log/LogLevel.hpp>
#include <SPIN/Log/Sinks/ISink.hpp>

namespace SPIN
{
    namespace Log
    {
        template<std::size_t bufferSize>
        class ILogger
        {
            protected:
                char _buffer[bufferSize] = { 0 };
                SPIN::Log::Sinks::ISink** _sinks = nullptr;
                std::size_t _numberOfSinks = 0;

                virtual void LogExpansion(SPIN::Log::LogLevel, const char*, va_list) = 0;
            public:
                ILogger() = default;
                ILogger(const ILogger<bufferSize>& obj)
                {
                    this->_sinks = (SPIN::Log::Sinks::ISink**)malloc(obj._numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
                    if (this->_sinks == nullptr)
                    {
#ifndef ARDUINO
                        throw std::exception();
#endif
                        return;
                    }
                    memcpy((void*)(this->_sinks), (const void*)(obj._sinks), obj._numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
                    this->_numberOfSinks = obj._numberOfSinks;

                    memcpy((void*)(this->_buffer), (const void*)(obj._buffer), bufferSize * sizeof(char));
                }
                ILogger(ILogger<bufferSize>&& deadObj) noexcept
                {
                    this->_sinks = deadObj._sinks;
                    this->_numberOfSinks = deadObj._numberOfSinks;
                    memcpy((void*)(this->_buffer), (const void*)(deadObj._buffer), bufferSize * sizeof(char));

                    deadObj._sinks = nullptr;
                    deadObj._numberOfSinks = 0;
                }

                void Log(SPIN::Log::LogLevel logLevel, const char* fmt, ...)
                {
                    va_list args;
                    va_start(args, fmt);

                    this->LogExpansion(logLevel, fmt, args);

                    va_end(args);
                }
                void Verbose(const char* fmt, ...)
                {
                    va_list args;
                    va_start(args, fmt);

                    this->LogExpansion(SPIN::Log::LogLevel::Verbose, fmt, args);

                    va_end(args);
                }
                void Debug(const char* fmt, ...)
                {
                    va_list args;
                    va_start(args, fmt);

                    this->LogExpansion(SPIN::Log::LogLevel::Debug, fmt, args);

                    va_end(args);
                }
                void Information(const char* fmt, ...)
                {
                    va_list args;
                    va_start(args, fmt);

                    this->LogExpansion(SPIN::Log::LogLevel::Information, fmt, args);

                    va_end(args);
                }
                void Warning(const char* fmt, ...)
                {
                    va_list args;
                    va_start(args, fmt);

                    this->LogExpansion(SPIN::Log::LogLevel::Warning, fmt, args);

                    va_end(args);
                }
                void Error(const char* fmt, ...)
                {
                    va_list args;
                    va_start(args, fmt);

                    this->LogExpansion(SPIN::Log::LogLevel::Error, fmt, args);

                    va_end(args);
                }
                void Fatal(const char* fmt, ...)
                {
                    va_list args;
                    va_start(args, fmt);

                    this->LogExpansion(SPIN::Log::LogLevel::Fatal, fmt, args);

                    va_end(args);
                }

                void Flush()
                {
                    if (this->_sinks == nullptr)
                    {
                        return;
                    }

                    for (std::size_t i = 0; i < this->_numberOfSinks; i++)
                    {
                        this->_sinks[i]->Flush();
                    }
                }

                ILogger<bufferSize>& operator=(const ILogger<bufferSize>& obj)
                {
                    this->_sinks = (SPIN::Log::Sinks::ISink**)malloc(obj._numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
                    if (this->_sinks == nullptr)
                    {
#ifndef ARDUINO
                        throw std::exception();
#endif
                        return *this;
                    }
                    memcpy((void*)(this->_sinks), (const void*)(obj._sinks), obj._numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
                    this->_numberOfSinks = obj._numberOfSinks;

                    memcpy((void*)(this->_buffer), (const void*)(obj._buffer), bufferSize * sizeof(char));

                    return *this;
                }
                ILogger<bufferSize>& operator=(ILogger<bufferSize>&& deadObj) noexcept
                {
                    this->_sinks = deadObj._sinks;
                    this->_numberOfSinks = deadObj._numberOfSinks;
                    memcpy((void*)(this->_buffer), (const void*)(deadObj._buffer), bufferSize * sizeof(char));

                    deadObj._sinks = nullptr;
                    deadObj._numberOfSinks = 0;

                    return *this;
                }
        };
    }
}
#endif
