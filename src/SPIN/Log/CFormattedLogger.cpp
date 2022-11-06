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

#include <SPIN/Log/CFormattedLogger.hpp>

#ifdef ARDUINO
    #include <stdlib.h>
    #include <string.h>
#else
    #include <cstdlib>
    #include <cstring>
#endif


bool SPIN::Log::Factory::CFormattedLoggerFactory::DoubleCapacityIfNeeded()
{
    if (this->_sinks == nullptr)
    {
        this->_sinks = (SPIN::Log::Sinks::ISink**)malloc(2 * sizeof(SPIN::Log::Sinks::ISink*));
        if (this->_sinks == nullptr)
        {
            return false;
        }
        this->_sizeOfSinks = 2;
    }

    if (this->_numberOfSinks <= this->_sizeOfSinks)
    {
        return true;
    }

    auto** temp = (SPIN::Log::Sinks::ISink**)realloc(this->_sinks, this->_sizeOfSinks * 2 * sizeof(SPIN::Log::Sinks::ISink*));
    if (temp == nullptr)
    {
        return false;
    }

    this->_sinks = temp;
    this->_numberOfSinks *= 2;

    return true;
}


SPIN::Log::Factory::CFormattedLoggerFactory::CFormattedLoggerFactory(const SPIN::Log::Factory::CFormattedLoggerFactory& obj)
{
    this->_sinks = (SPIN::Log::Sinks::ISink**)malloc(obj._numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
    if (this->_sinks == nullptr)
    {
#ifndef ARDUINO
        throw std::exception();
#endif
        return;
    }
    this->_numberOfSinks = obj._numberOfSinks;
    this->_sizeOfSinks = obj._sizeOfSinks;

    memcpy((void*)(this->_sinks), (const void*)(obj._sinks), obj._sizeOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
}
SPIN::Log::Factory::CFormattedLoggerFactory::CFormattedLoggerFactory(SPIN::Log::Factory::CFormattedLoggerFactory&& deadObj) noexcept
{
    this->_sinks = deadObj._sinks;
    this->_numberOfSinks = deadObj._numberOfSinks;
    this->_sizeOfSinks = deadObj._sizeOfSinks;

    deadObj._sinks = nullptr;
    deadObj._numberOfSinks = 0;
    deadObj._sizeOfSinks = 0;
}


SPIN::Log::Factory::CFormattedLoggerFactory& SPIN::Log::Factory::CFormattedLoggerFactory::AddSink(SPIN::Log::Sinks::ISink* sink)
{
    if (!this->DoubleCapacityIfNeeded())
    {
#ifndef ARDUINO
        throw std::exception();
#endif
        return *this;
    }

    this->_sinks[this->_numberOfSinks++] = sink;

    return *this;
}


SPIN::Log::Factory::CFormattedLoggerFactory& SPIN::Log::Factory::CFormattedLoggerFactory::operator=(const SPIN::Log::Factory::CFormattedLoggerFactory& obj)
{
    this->_sinks = (SPIN::Log::Sinks::ISink**)malloc(obj._numberOfSinks * sizeof(SPIN::Log::Sinks::ISink*));
    if (this->_sinks == nullptr)
    {
#ifndef ARDUINO
        throw std::exception();
#endif
        return *this;
    }
    this->_numberOfSinks = obj._numberOfSinks;
    this->_sizeOfSinks = obj._sizeOfSinks;

    memcpy((void*)(this->_sinks), (const void*)(obj._sinks), obj._sizeOfSinks * sizeof(SPIN::Log::Sinks::ISink*));

    return *this;
}
SPIN::Log::Factory::CFormattedLoggerFactory& SPIN::Log::Factory::CFormattedLoggerFactory::operator=(SPIN::Log::Factory::CFormattedLoggerFactory&& deadObj) noexcept
{
    this->_sinks = deadObj._sinks;
    this->_numberOfSinks = deadObj._numberOfSinks;
    this->_sizeOfSinks = deadObj._sizeOfSinks;

    deadObj._sinks = nullptr;
    deadObj._numberOfSinks = 0;
    deadObj._sizeOfSinks = 0;

    return *this;
}


SPIN::Log::Factory::CFormattedLoggerFactory::~CFormattedLoggerFactory()
{
    if (this->_sinks != nullptr)
    {
        free((void*)(this->_sinks));
    }

    this->_sinks = nullptr;
    this->_numberOfSinks = 0;
    this->_sizeOfSinks = 0;
}