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

#if !defined(__LOGGER__SPIN__LOG__SINKS_FILESINK__H__) && defined(__cplusplus)
#define __LOGGER__SPIN__LOG__SINKS_FILESINK__H__

#ifdef ARDUINO
    #include <Arduino.h>
    #include <SD.h>
#else
    #include <ostream>
#endif

#include <SPIN/Log/LogLevel.hpp>
#include <SPIN/Log/Sinks/ISink.hpp>

namespace SPIN
{
    namespace Log
    {
        namespace Sinks
        {
            namespace Factory
            {
                class FileSinkFactory;
            }

            class FileSink : public SPIN::Log::Sinks::ISink
            {
                private:
                    char* _fileNameFmt = nullptr;
                    std::size_t _fileNameFmtSize = 0;
                    char* _fileName = nullptr;
                    std::size_t _fileNameSize = 0;
                    uint32_t _counter = 0;
                    bool _fileOpen = false;
#ifdef ARDUINO
                    File _fptr;
#else
                    FILE* _fptr = nullptr;
#endif

                    FileSink(char*);

                    bool SetFileNameFmt(char*);
                    bool OpenNextFile();
                    void CloseFile();

                    friend class SPIN::Log::Sinks::Factory::FileSinkFactory;

                public:
                    FileSink() = delete;
                    FileSink(const FileSink&);
                    FileSink(FileSink&&) noexcept;

                    void Handle(SPIN::Log::LogLevel, const char*) override;
                    void Flush() override;

                    FileSink& operator=(const FileSink&);
                    FileSink& operator=(FileSink&&) noexcept;

                    ~FileSink();
            };

            namespace Factory
            {
                class FileSinkFactory
                {
                    private:
                        char* _fileNameFmt = nullptr;
                        std::size_t _fileNameFmtSize = 0;

                    public:
                        FileSinkFactory();
                        FileSinkFactory(const FileSinkFactory&);
                        FileSinkFactory(FileSinkFactory&&) noexcept;

                        FileSinkFactory& SetFileNameFormatter(const char*);

                        SPIN::Log::Sinks::FileSink Build();

                        FileSinkFactory& operator=(const FileSinkFactory&);
                        FileSinkFactory& operator=(FileSinkFactory&&) noexcept;

                        ~FileSinkFactory();
                };
            }
        }
    }
}

#endif