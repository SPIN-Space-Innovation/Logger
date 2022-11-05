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
    #include <SD.h>
    #include <SoftwareSerial.h>
#else
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <iostream>
#endif

#include <SPIN/Log/LogLevel.hpp>

#include <SPIN/Log/Sinks/ISink.hpp>
#include <SPIN/Log/Sinks/SerialSink.hpp>

namespace SPIN
{
    namespace Log
    {
        namespace Sinks
        {
            class FileSink;
            class FileSinkConfigurations
            {
                private:
                    const char* _fileNameFormatter = (char*)NULL;

                    friend class SPIN::Log::Sinks::FileSink;
                public:
                    FileSinkConfigurations() = default;
                    FileSinkConfigurations(const FileSinkConfigurations& conf)
                    {
                        this->SetFileNameFormatter(conf._fileNameFormatter);
                    }

                    FileSinkConfigurations& SetFileNameFormatter(const char* fmt)
                    {
                        this->_fileNameFormatter = fmt;
                        return *this;
                    }
            };

            class FileSink : public SPIN::Log::Sinks::ISink
            {
                private:
                    const char* _fileNameFormatter = (const char*)NULL;
                    char* _fileNameBuffer = (char*)NULL;
                    size_t _fileNameBufferSize = 0;
                    uint16_t _counter = 0;
                    bool _fileOpen = false;

#ifdef ARDUINO
                    File _fptr;
#else
                    FILE* _fptr = (FILE*)NULL;
#endif
                    const char _tag[6][8] = {
                        "[VER]: ",
                        "[DEB]: ",
                        "[INF]: ",
                        "[WAR]: ",
                        "[ERR]: ",
                        "[FAT]: "
                    };

                    FileSink() = delete;

                    void OpenFile()
                    {
                        if (!this->_fileNameBuffer)
                        {
                            size_t fileNameFormatterSize = strlen(this->_fileNameFormatter);
                            size_t fileNameBufferSize = fileNameFormatterSize + 4 * 5 + 1;

                            this->_fileNameBuffer = (char*)malloc(fileNameFormatterSize * sizeof(char));

                            if (!this->_fileNameBuffer)
                                return;

                            this->_fileNameBufferSize = fileNameBufferSize;
                        }

                        bool fileFound;

#ifdef ARDUINO
                        do
                        {
                            snprintf(this->_fileNameBuffer,
                                this->_fileNameBufferSize,
                                this->_fileNameFormatter,
                                this->_counter,
                                this->_counter,
                                this->_counter,
                                this->_counter);
                            this->_counter++;

                            fileFound = SD.exists(this->_fileNameBuffer);
                        } while (fileFound);

                        this->_fptr = SD.open(this->_fileNameBuffer, FILE_WRITE);
#else
                        do
                        {
                            snprintf(this->_fileNameBuffer,
                                this->_fileNameBufferSize,
                                this->_fileNameFormatter,
                                this->_counter,
                                this->_counter,
                                this->_counter,
                                this->_counter);

                            this->_counter++;

                            FILE* fptr;
                            fileFound = (fptr = fopen(this->_fileNameBuffer, "r")) != NULL;

                            if (fileFound)
                            {
                                fclose(fptr);
                            }
                        } while (fileFound);

                        this->_fptr = fopen(this->_fileNameBuffer, "w");
#endif

                        this->_fileOpen = true;
                    }

                public:
                    FileSink(FileSinkConfigurations& conf) :
                        _fileNameFormatter{ conf._fileNameFormatter }
                    {
                        this->OpenFile();
                    }

                    FileSink(const FileSink& sink) : _fileNameFormatter{ sink._fileNameFormatter }
                    {
                        this->OpenFile();
                    }

                    void Handle(SPIN::Log::LogLevel logLevel, const char* message) override
                    {
                        if (!this->_fileOpen)
                        {
                            this->OpenFile();
                        }

                        const char* tag = _tag[(uint8_t)logLevel];
#ifdef ARDUINO
                        this->_fptr.print(tag);
                        this->_fptr.println(message);
#else
                        fprintf(this->_fptr, "%s%s\n", tag, message);
#endif
                    }

                    void Flush() override
                    {
#ifdef ARDUINO
                        this->_fptr.flush();
#else
                        fflush(this->_fptr);
#endif
                    }
            };
        } // namespace Sinks
        
        class ILogger
        {
            private:
                virtual void LogEx(SPIN::Log::LogLevel logLevel, const char* message, va_list args) = 0;

            public:
                virtual void Log(SPIN::Log::LogLevel logLevel, const char* message, ...) = 0;
                virtual void Verbose(const char* message, ...) = 0;
                virtual void Debug(const char* message, ...) = 0;
                virtual void Information(const char* message, ...) = 0;
                virtual void Warning(const char* message, ...) = 0;
                virtual void Error(const char* message, ...) = 0;
                virtual void Fatal(const char* message, ...) = 0;
        };

        class SimpleLoggerFactory;
        class SimpleLogger : public ILogger
        {
            private:
                char* _buffer = (char*)NULL;
                uint16_t _bufferSize = 0;
                SPIN::Log::Sinks::ISink** _sinks = (SPIN::Log::Sinks::ISink**)NULL;
                uint16_t _numberOfSinks = 0;

                SimpleLogger() = delete;
                SimpleLogger(uint16_t bufferSize, SPIN::Log::Sinks::ISink** sinks, uint16_t numberOfSinks)
                {
                    this->_buffer = (char*)malloc(bufferSize * sizeof(char));
                    if (!this->_buffer)
                    {
                        return;
                    }
                    this->_bufferSize = bufferSize;

                    this->_sinks = (SPIN::Log::Sinks::ISink**)malloc(numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
                    if (!this->_sinks)
                    {
                        free((void*)(this->_buffer));
                        this->_buffer = (char*)NULL;
                        this->_bufferSize;
                    }
                    memcpy((void*)(this->_sinks), (const void*)sinks, numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
                    this->_numberOfSinks = numberOfSinks;
                }

                void LogEx(SPIN::Log::LogLevel logLevel, const char* message, va_list args)
                {
                    vsnprintf(this->_buffer, this->_bufferSize, message, args);

                    for (uint16_t i = 0; i < this->_numberOfSinks; i++)
                    {
                        this->_sinks[i]->Handle(logLevel, this->_buffer);
                    }
                }

                friend class SPIN::Log::SimpleLoggerFactory;

            public:
                void Log(SPIN::Log::LogLevel logLevel, const char* message, ...) override
                {
                    va_list args;
                    va_start(args, message);

                    this->LogEx(logLevel, message, args);

                    va_end(args);
                }

                void Verbose(const char* message, ...) override
                {
                    va_list args;
                    va_start(args, message);

                    this->LogEx(SPIN::Log::LogLevel::Verbose, message, args);

                    va_end(args);
                }
                void Debug(const char* message, ...) override
                {
                    va_list args;
                    va_start(args, message);

                    this->LogEx(SPIN::Log::LogLevel::Debug, message, args);

                    va_end(args);
                }
                void Information(const char* message, ...) override
                {
                    va_list args;
                    va_start(args, message);

                    this->LogEx(SPIN::Log::LogLevel::Information, message, args);

                    va_end(args);
                }
                void Warning(const char* message, ...) override
                {
                    va_list args;
                    va_start(args, message);

                    this->LogEx(SPIN::Log::LogLevel::Warning, message, args);

                    va_end(args);
                }
                void Error(const char* message, ...) override
                {
                    va_list args;
                    va_start(args, message);

                    this->LogEx(SPIN::Log::LogLevel::Error, message, args);

                    va_end(args);
                }
                void Fatal(const char* message, ...) override
                {
                    va_list args;
                    va_start(args, message);

                    this->LogEx(SPIN::Log::LogLevel::Fatal, message, args);

                    va_end(args);
                }
        };

        class SimpleLoggerFactory
        {
            SPIN::Log::Sinks::ISink** _sinks = (SPIN::Log::Sinks::ISink**)NULL;
            uint16_t _numberOfSinks = 0;
            uint16_t _sizeOfSinks = 0;

            bool DoubleSinksArray(uint16_t minimumNumberOfSinks = 1)
            {
                if (!minimumNumberOfSinks)
                {
                    minimumNumberOfSinks = 1;
                }

                if (!this->_sinks)
                {
                    this->_sinks = (SPIN::Log::Sinks::ISink**)malloc(minimumNumberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));

                    if (!this->_sinks)
                    {
                        return false;
                    }

                    this->_sizeOfSinks = 1;

                    return true;
                }

                uint16_t newSize = (2 * this->_sizeOfSinks < minimumNumberOfSinks) ? minimumNumberOfSinks : 2 * this->_sizeOfSinks;
                SPIN::Log::Sinks::ISink** newPtr = (SPIN::Log::Sinks::ISink**)realloc(this->_sinks, newSize * sizeof(SPIN::Log::Sinks::ISink*));
                if (!newPtr)
                {
                    return false;
                }

                uint16_t uninitializedSinks = newSize - this->_sizeOfSinks;

                memset((void*)(&(newPtr[this->_numberOfSinks + 1])), 0, uninitializedSinks * sizeof(SPIN::Log::Sinks::ISink*));

                this->_sinks = newPtr;
                this->_sizeOfSinks = newSize;

                return true;
            }

        public:
            SimpleLoggerFactory()
            {
                this->DoubleSinksArray(1);
            }

            SimpleLoggerFactory(const SimpleLoggerFactory& factory)
            {
                this->DoubleSinksArray(factory._numberOfSinks);

                memcpy((void*)(this->_sinks), (const void*)(factory._sinks), factory._numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
            }

            SimpleLoggerFactory& AddSink(SPIN::Log::Sinks::ISink* sink)
            {
                if (this->_numberOfSinks == this->_sizeOfSinks && !DoubleSinksArray())
                {
                    return *this;
                }

                this->_sinks[this->_numberOfSinks++] = sink;

                return *this;
            }

            SPIN::Log::ILogger* Build()
            {
                SPIN::Log::ILogger* logger = new SPIN::Log::SimpleLogger(1024, this->_sinks, this->_numberOfSinks);

                return logger;
            }

            ~SimpleLoggerFactory()
            {
                if (this->_sinks)
                {
                    free((void*)(this->_sinks));
                }

                this->_sinks = (SPIN::Log::Sinks::ISink**)NULL;
                this->_numberOfSinks = 0;
                this->_sizeOfSinks = 0;
            }
        };
    } // namespace Log
    
} // namespace SPIN


#endif/*!__LOGGER__LOGGER__H__*/