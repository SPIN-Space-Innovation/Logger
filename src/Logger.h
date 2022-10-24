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
#define __LOGGER__LOGGER__H__ (0 * 10000 + 1 * 100 + 0)

#include <stdarg.h>
#include <stdint.h>

#ifdef ARDUINO
    #include <Arduino.h>
    #include <SoftwareSerial.h>
#else
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <iostream>
#endif

namespace SPIN
{
    namespace Log
    {
        enum class LogLevel : uint8_t
        {
            Verbose = 0,
            Debug = 1,
            Information = 2,
            Warning = 3,
            Error = 4,
            Fatal = 5
        };

        namespace Sinks
        {
            class ISink
            {
                public:
                    virtual void Handle(SPIN::Log::LogLevel, const char* message) = 0;
                    virtual void Flush() = 0;
            };

            class SerialSink;
            class SerialSinkConfigurations
            {
                private:
#ifdef ARDUINO
                    Stream* _stream = (Stream*)NULL;
#else
                    std::ostream* _stream = (std::ostream*)NULL;
#endif
                    bool _coloured = false;

                    friend class SPIN::Log::Sinks::SerialSink;
                public:
                    SerialSinkConfigurations() = default;
                    SerialSinkConfigurations(const SerialSinkConfigurations& conf) :
                        _stream { conf._stream }, _coloured { conf._coloured } { }

#ifdef ARDUINO
                    SerialSinkConfigurations& SetStream(HardwareSerial* stream)
                    {
                        this->_stream = stream;

                        return *this;
                    }
                    SerialSinkConfigurations& SetStream(SoftwareSerial* stream)
                    {
                        this->_stream = stream;

                        return *this;
                    }
#else
                    SerialSinkConfigurations& SetStream(std::ostream* stream)
                    {
                        this->_stream = stream;

                        return *this;
                    }
#endif

                    SerialSinkConfigurations& SetColoured(bool coloured)
                    {
                        this->_coloured = coloured;

                        return *this;
                    }
            };

            class SerialSink : public SPIN::Log::Sinks::ISink
            {
                private:
#ifdef ARDUINO
                    Stream* _stream = (Stream*)NULL;
#else
                    std::ostream* _stream = (std::ostream*)NULL;
#endif
                    bool _coloured = false;

                    const char _uncolouredTag[6][8] = {
                        "[VER]: ",
                        "[DEB]: ",
                        "[INF]: ",
                        "[WAR]: ",
                        "[ERR]: ",
                        "[FAT]: "
                    };
                    const char _colouredTag[6][17] = {
                        "[\e[37mVER\e[0m]: ",
                        "[\e[36mDEB\e[0m]: ",
                        "[\e[93mINF\e[0m]: ",
                        "[\e[34mWAR\e[0m]: ",
                        "[\e[31mERR\e[0m]: ",
                        "[\e[91mFAT\e[0m]: "
                    };

                    SerialSink() = delete;
                    SerialSink() = delete;

                public:
                    SerialSink(const SerialSink& sink) :
                        _stream { sink._stream }, _coloured { sink._coloured } { }
                    SerialSink(SerialSinkConfigurations& conf) :
                        _stream { conf._stream }, _coloured { conf._coloured } { }

                    void Handle(SPIN::Log::LogLevel logLevel, const char* message) override
                    {
                        const char* tag = (this->_coloured) ? _colouredTag[(uint8_t)logLevel] : _uncolouredTag[(uint8_t)logLevel];
#ifdef ARDUINO
                        this->_stream->print(tag);
                        this->_stream->println(message);
#else
                        *(this->_stream) << tag << message << '\n';
#endif
                    }

                    void Flush() override
                    {
#ifdef ARDUINO
                        this->_stream->flush();
#else
                        *(this->_stream) << std::flush;
#endif
                    }
            };
        } // namespace Sinks
        
    } // namespace Log
    
} // namespace SPIN


#endif/*!__LOGGER__LOGGER__H__*/