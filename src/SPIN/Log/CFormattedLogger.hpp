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

#if !defined(__LOGGER__SPIN__LOG__CFORMATTEDLOGGER__H__) && defined(__cplusplus)
#define __LOGGER__SPIN__LOG__CFORMATTEDLOGGER__H__

#ifdef ARDUINO
    #include <stdarg.h>
    #include <stddef.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
#else
    #include <cstdarg>
    #include <cstddef>
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <exception>
#endif

#include <SPIN/Log/LogLevel.hpp>
#include <SPIN/Log/Sinks/ISink.hpp>
#include <SPIN/Log/ILogger.hpp>

namespace SPIN
{
    namespace Log
    {
        namespace Factory
        {
            class CFormattedLoggerFactory;
        }

        template<std::size_t bufferSize>
        class CFormattedLogger : public SPIN::Log::ILogger<bufferSize>
        {
            private:
                CFormattedLogger(SPIN::Log::Sinks::ISink** sinks, std::size_t numberOfSinks)
                {
                    this->_sinks = (SPIN::Log::Sinks::ISink**)malloc(numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
                    if (this->_sinks == nullptr)
                    {
#ifndef ARDUINO
                        throw std::exception();
#endif
                        return;
                    }
                    memcpy((void*)(this->_sinks), (const void*)sinks, numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
                    this->_numberOfSinks = numberOfSinks;
                }

                friend class SPIN::Log::Factory::CFormattedLoggerFactory;

            protected:
                void LogExpansion(SPIN::Log::LogLevel logLevel, const char* fmt, va_list args)
                {
                    vsnprintf(this->_buffer, bufferSize, fmt, args);

                    for (std::size_t i = 0; i < this->_numberOfSinks; i++)
                    {
                        this->_sinks[i]->Handle(logLevel, this->_buffer);
                    }
                }
        };

        namespace Factory
        {
            class CFormattedLoggerFactory
            {
                private:
                    SPIN::Log::Sinks::ISink** _sinks = nullptr;
                    std::size_t _numberOfSinks = 0;
                    std::size_t _sizeOfSinks = 0;

                    bool DoubleCapacityIfNeeded();
                public:
                    CFormattedLoggerFactory() = default;
                    CFormattedLoggerFactory(const CFormattedLoggerFactory&);
                    CFormattedLoggerFactory(CFormattedLoggerFactory&&) noexcept;

                    CFormattedLoggerFactory& AddSink(SPIN::Log::Sinks::ISink*);

                    template<std::size_t bufferSize>
                    SPIN::Log::CFormattedLogger<bufferSize> Build()
                    {
                        return SPIN::Log::CFormattedLogger<bufferSize>(_sinks, _numberOfSinks);
                    }

                    CFormattedLoggerFactory& operator=(const CFormattedLoggerFactory&);
                    CFormattedLoggerFactory& operator=(CFormattedLoggerFactory&&) noexcept;

                    ~CFormattedLoggerFactory();
            };
        }
    }
}

#endif