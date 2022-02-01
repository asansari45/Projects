#include <cstdio>
#include <cstdlib>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <new>
#include "InterpreterLog.h"

struct MemInfo
{
	const char* m_pFile;
	int m_Line;
	void* m_pMem;
	MemInfo* m_pNext;
};

static MemInfo* f_pHead = nullptr;

static void Insert(MemInfo* pNode)
{
	pNode->m_pNext = f_pHead;
	f_pHead = pNode;
}

static MemInfo* Search(void* pMem)
{
	for (MemInfo* pNode = f_pHead; pNode != nullptr; pNode = pNode->m_pNext)
	{
		if (pNode->m_pMem == pMem)
		{
			return pNode;
		}
	}

	return nullptr;
}

static void Remove(MemInfo* pNode)
{
	if (pNode == f_pHead)
	{
		f_pHead = f_pHead->m_pNext;
		return;
	}

	MemInfo* pPrev = f_pHead;
	for (; pPrev->m_pNext != pNode; pPrev = pPrev->m_pNext);
	pPrev->m_pNext = pNode->m_pNext;
}

void* operator new(size_t size, const char* file, int line)
{
	// Allocate the memory
	void* p = malloc(size);
	assert(p != nullptr);

	// Better not be in there
	assert(Search(p) == nullptr);

	// Allocate the info.
	MemInfo* pMemInfo = reinterpret_cast<MemInfo*>(malloc(sizeof(MemInfo)));
	assert(pMemInfo != nullptr);
	pMemInfo->m_pFile = file;
	pMemInfo->m_Line = line;
	pMemInfo->m_pMem = p;
	pMemInfo->m_pNext = nullptr;
	Insert(pMemInfo);
	return p;
}

void* operator new[](size_t size, const char* file, int line)
{
	// Allocate the memory
	void* p = malloc(size);
	assert(p != nullptr);

	// Better not be in there
	assert(Search(p) == nullptr);

	// Allocate the info.
	MemInfo* pMemInfo = reinterpret_cast<MemInfo*>(malloc(sizeof(MemInfo)));
	assert(pMemInfo != nullptr);
	pMemInfo->m_pFile = file;
	pMemInfo->m_Line = line;
	pMemInfo->m_pMem = p;
	pMemInfo->m_pNext = nullptr;
	Insert(pMemInfo);
	return p;
}

void* operator new(size_t size)
{
	return operator new(size, "<Unknown>", 0);
}


void operator delete (void* p)
{
	if (p == nullptr)
	{
		return;
	}

	// Better be there
	MemInfo* pNode = Search(p);
	assert(pNode != nullptr);

	// Free up info
	Remove(pNode);
	free(pNode->m_pMem);
	free(pNode);
}

void operator delete[](void* p)
{
	if (p == nullptr)
	{
		return;
	}
	// Better be there
	MemInfo* pNode = Search(p);
	assert(pNode != nullptr);

	// Free up info
	Remove(pNode);
	free(pNode->m_pMem);
	free(pNode);
}

void PrintStatistics(bool NoUnknowns)
{
	bool ons = true;
	char buf[512];
	for (MemInfo* pNode = f_pHead; pNode != nullptr; pNode=pNode->m_pNext)
	{
		if (NoUnknowns && strcmp(pNode->m_pFile, "<Unknown>") != 0)
		{
			if (ons)
			{
				sprintf_s(buf, sizeof(buf),"Unfreed Memory!");
				Interpreter::Log::GetInst()->AddMessage(buf);
				sprintf_s(buf, sizeof(buf), "FILE     LINE  MEMORY");
				Interpreter::Log::GetInst()->AddMessage(buf);
				ons = false;
			}
			
			sprintf_s(buf, sizeof(buf), "%-30s  %05d   %p", pNode->m_pFile, pNode->m_Line, pNode->m_pMem);
			Interpreter::Log::GetInst()->AddMessage(buf);
		}
	}
}