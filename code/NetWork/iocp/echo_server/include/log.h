#ifndef ___LOG_H_INCLUDED___
#define ___LOG_H_INCLUDED___

#include "INCLUDE/sync_simple.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- CLASS ----------------------------------------------------------
// A simple class to log messages to the console
// considering multithreading
class Log {
private:
	static QMutex		m_qMutex;

public:
	static void LogMessage(const wchar_t *str);
	static void LogMessage(const wchar_t *str, long num);
	static void LogMessage(const wchar_t *str, long num1, long num2);
	static void LogMessage(const wchar_t *str, long num, wchar_t *str1);
	static void LogMessage(const wchar_t *str, wchar_t *str1);
	static void LogMessage(const wchar_t *str, wchar_t *str1, wchar_t *str2);
};

///////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------- IMPLEMENTATION -------------------------------------------------

QMutex Log::m_qMutex;

void Log::LogMessage(const wchar_t *str, long num1, long num2) {
	m_qMutex.Lock();
	fwprintf(stderr, str, num1, num2);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str, long num, wchar_t *str1) {
	m_qMutex.Lock();
	fwprintf(stderr, str, num, str1);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str, wchar_t *str1, wchar_t *str2) {
	m_qMutex.Lock();
	fwprintf(stderr, str, str1, str2);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str, wchar_t *str1) {
	m_qMutex.Lock();
	fwprintf(stderr, str, str1);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str, long num) {
	m_qMutex.Lock();
	fwprintf(stderr, str, num);
	m_qMutex.Unlock();
}

void Log::LogMessage(const wchar_t *str) {
	m_qMutex.Lock();
	fwprintf(stderr,str);
	m_qMutex.Unlock();
}

#endif