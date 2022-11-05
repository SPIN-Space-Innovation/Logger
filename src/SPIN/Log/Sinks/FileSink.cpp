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

#include <SPIN/Log/Sinks/FileSink.hpp>


#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

#ifdef ARDUINO
    #include <Arduino.h>
    #include <stdint.h>
#else
    #include <ostream>
    #include <cstdint>
#endif

static const char tags[6][7] = {
    "[VER]:",
    "[DEB]:",
    "[INF]:",
    "[WAR]:",
    "[ERR]:",
    "[FAT]:"
};


SPIN::Log::Sinks::FileSink::FileSink(char* fmt)
{
    if (!this->SetFileNameFmt(fmt))
    {
#ifndef ARDUINO
        throw std::exception();
#endif
        return;
    }
}
SPIN::Log::Sinks::FileSink::FileSink(const SPIN::Log::Sinks::FileSink& obj)
{
    if (!this->SetFileNameFmt(obj._fileNameFmt))
    {
#ifndef ARDUINO
        throw std::exception();
#endif
        return;
    }
}
SPIN::Log::Sinks::FileSink::FileSink(SPIN::Log::Sinks::FileSink&& deadObj) noexcept
{
    this->_fileNameFmt = deadObj._fileNameFmt;
    this->_fileNameFmtSize = deadObj._fileNameFmtSize;
    this->_fileName = deadObj._fileName;
    this->_fileNameSize = deadObj._fileNameSize;
    this->_counter = deadObj._counter;
    this->_fileOpen = deadObj._fileOpen;
    this->_fptr = deadObj._fptr;

    deadObj._fileNameFmt = nullptr;
    deadObj._fileName = nullptr;
    deadObj._fileOpen = false;
}


bool SPIN::Log::Sinks::FileSink::SetFileNameFmt(char* fmt)
{
    std::size_t fmtSize = strlen(fmt);
    this->_fileNameFmt = (char*)malloc((fmtSize + 1) * sizeof(char));
    if (this->_fileNameFmt == nullptr)
    {
        return false;
    }
    this->_fileNameFmtSize = fmtSize;

    memcpy((void*)(this->_fileNameFmt), (const void*)fmt, (fmtSize + 1) * sizeof(char));

    return true;
}
bool SPIN::Log::Sinks::FileSink::OpenNextFile()
{
    if (this->_fileNameFmt == nullptr)
    {
        return false;
    }

    if (this->_fileName == nullptr)
    {
        std::size_t fileNameSize = this->_fileNameFmtSize + 4 * 10;

        this->_fileName = (char*)malloc((fileNameSize + 1) * sizeof(char));
        if (this->_fileName == nullptr)
        {
#ifndef ARDUINO
            throw std::exception();
#endif
            return false;
        }
        this->_fileNameSize = fileNameSize;
    }

    this->CloseFile();

    bool fileFound;
    do
    {
        snprintf(this->_fileName,
                 (const std::size_t)(this->_fileName + 1),
                 this->_fileNameFmt,
                 this->_counter,
                 this->_counter,
                 this->_counter,
                 this->_counter);

        this->_counter++;

#ifdef ARDUINO
        fileFound = SD.exists(this->_fileName);
#else
        FILE* fptr;
        fileFound = (fptr = fopen(this->_fileName, "r")) != NULL;

        if (fileFound)
        {
            fclose(fptr);
        }
#endif
    } while (fileFound);

#ifdef ARDUINO
    this->_fptr = SD.open(this->_fileName, FILE_WRITE);
#else
    this->_fptr = fopen(this->_fileName, "w");
#endif
    this->_fileOpen = true;

    return true;
}
void SPIN::Log::Sinks::FileSink::CloseFile()
{
    if (!this->_fileOpen)
    {
        return;
    }

#ifdef ARDUINO
    this->_fptr.close();
#else
    fclose(this->_fptr);
#endif
}


void SPIN::Log::Sinks::FileSink::Handle(SPIN::Log::LogLevel logLevel, const char* message)
{
    const char* tag = tags[(uint8_t)logLevel];

    if (!this->_fileOpen && !this->OpenNextFile())
    {
#ifndef ARDUINO
        throw std::exception();
#endif
        return;
    }

#ifdef ARDUINO
    this->_fptr.print(tag);
    this->_fptr.print(' ');
    this->_fptr.println(message);
#else
    fprintf(this->_fptr, "%s %s\n", tag, message);
#endif
}
void SPIN::Log::Sinks::FileSink::Flush()
{
    if (!this->_fileOpen)
    {
        return;
    }

#ifdef ARDUINO
    this->_fptr.flush();
#else
    fflush(this->_fptr);
#endif
}


SPIN::Log::Sinks::FileSink& SPIN::Log::Sinks::FileSink::operator=(const SPIN::Log::Sinks::FileSink& obj)
{
    if (!this->SetFileNameFmt(obj._fileNameFmt))
    {
#ifndef ARDUINO
        throw std::exception();
#endif
    }

    return *this;
}
SPIN::Log::Sinks::FileSink& SPIN::Log::Sinks::FileSink::operator=(SPIN::Log::Sinks::FileSink&& deadObj)
{
    this->_fileNameFmt = deadObj._fileNameFmt;
    this->_fileNameFmtSize = deadObj._fileNameFmtSize;
    this->_fileName = deadObj._fileName;
    this->_fileNameSize = deadObj._fileNameSize;
    this->_counter = deadObj._counter;
    this->_fileOpen = deadObj._fileOpen;
    this->_fptr = deadObj._fptr;

    deadObj._fileNameFmt = nullptr;
    deadObj._fileName = nullptr;
    deadObj._fileOpen = false;

    return *this;
}


SPIN::Log::Sinks::FileSink::~FileSink()
{
    this->CloseFile();

    if (this->_fileNameFmt != nullptr)
    {
        free((void*)(this->_fileNameFmt));
    }
    this->_fileNameFmt = nullptr;
    this->_fileNameFmtSize = 0;

    if (this->_fileName != nullptr)
    {
        free((void*)(this->_fileName));
    }
    this->_fileName = nullptr;
    this->_fileNameSize = 0;

    this->_counter = 0;
}



SPIN::Log::Sinks::Factory::FileSinkFactory::FileSinkFactory() = default;
SPIN::Log::Sinks::Factory::FileSinkFactory::FileSinkFactory(const SPIN::Log::Sinks::Factory::FileSinkFactory& obj)
{
    this->SetFileNameFormatter(obj._fileNameFmt);
}
SPIN::Log::Sinks::Factory::FileSinkFactory::FileSinkFactory(SPIN::Log::Sinks::Factory::FileSinkFactory&& deadObj) noexcept
{
    this->_fileNameFmt = deadObj._fileNameFmt;
    this->_fileNameFmtSize = deadObj._fileNameFmtSize;

    deadObj._fileNameFmt = nullptr;
    deadObj._fileNameFmtSize = 0;
}


SPIN::Log::Sinks::Factory::FileSinkFactory& SPIN::Log::Sinks::Factory::FileSinkFactory::SetFileNameFormatter(const char *fmt)
{
    if (fmt == nullptr)
    {
#ifndef ARDUINO
        throw std::exception();
#endif
        return *this;
    }

    std::size_t fmtSize = strlen(fmt);

    if (this->_fileNameFmt == nullptr)
    {
        this->_fileNameFmt = (char *)malloc((fmtSize + 1) * sizeof(char));
        if (this->_fileNameFmt == nullptr)
        {
#ifndef ARDUINO
            throw std::exception();
#endif
            return *this;
        }
        this->_fileNameFmtSize = fmtSize;
    }

    if (this->_fileNameFmtSize < fmtSize)
    {
        char* temp = (char*)realloc((void*)(this->_fileNameFmt), (fmtSize + 1) * sizeof(char));
        if (temp == nullptr)
        {
#ifndef ARDUINO
            throw std::exception();
#endif
            return *this;
        }
        this->_fileNameFmt = temp;
        this->_fileNameFmtSize = fmtSize;
    }

    memcpy((void*)(this->_fileNameFmt), (const void*)fmt, (fmtSize + 1) * sizeof(char));

    return *this;
}


SPIN::Log::Sinks::FileSink SPIN::Log::Sinks::Factory::FileSinkFactory::Build()
{
    auto sink = SPIN::Log::Sinks::FileSink(this->_fileNameFmt);

    return sink;
}


SPIN::Log::Sinks::Factory::FileSinkFactory& SPIN::Log::Sinks::Factory::FileSinkFactory::operator=(const SPIN::Log::Sinks::Factory::FileSinkFactory& obj)
{
    this->SetFileNameFormatter(obj._fileNameFmt);

    return *this;
}
SPIN::Log::Sinks::Factory::FileSinkFactory& SPIN::Log::Sinks::Factory::FileSinkFactory::operator=(SPIN::Log::Sinks::Factory::FileSinkFactory&& deadObj) noexcept
{
    this->_fileNameFmt = deadObj._fileNameFmt;
    this->_fileNameFmtSize = deadObj._fileNameFmtSize;

    deadObj._fileNameFmt = nullptr;
    deadObj._fileNameFmtSize = 0;

    return *this;
}


SPIN::Log::Sinks::Factory::FileSinkFactory::~FileSinkFactory()
{
    if (this->_fileNameFmt != nullptr)
    {
        free((void*)(this->_fileNameFmt));
    }

    this->_fileNameFmt = nullptr;
    this->_fileNameFmtSize = 0;
}