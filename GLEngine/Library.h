
#pragma once

#include <stdio.h>

template <class T>
void Swap(T &a, T &b)
{
	T c(a);
	a=b;
	b=c;
};

typedef unsigned int	count;
typedef unsigned int	size;
typedef unsigned int	index;	//	Index	: 0...N-1
typedef unsigned int	id;		//	ID		: 1.....N

//- Linked List ---------------------------------------------------------------

template <class NodeData>
class ListNode
{

public:

	ListNode()
	{
		m_lpNext = nullptr;
		pData = nullptr;
	}

	template <class ParamType>
	ListNode(const ParamType& Data)
	{
		m_lpNext = nullptr;

		ParamType* pParam = new ParamType();

		*pParam = (ParamType)Data;

		this->pData = (NodeData*)pParam;
	}

	~ListNode()
	{
		if (pData)
			delete pData;

		pData = nullptr;
	}

	void Destroy()
	{
		if (pData)
			delete pData;

		pData = nullptr;
	}

	NodeData* const	GetNodeDataPointer() const
	{
		return pData;
	}

	NodeData& GetNodeDataReference()
	{
		return *pData;
	}

	ListNode<NodeData>* getNext()
	{
		return m_lpNext;
	}

	void SetNext(const ListNode<NodeData>* Next)
	{
		this->m_lpNext = (ListNode<NodeData>*)Next;
	}

private:

	NodeData*				pData;

	ListNode<NodeData>*		m_lpNext;
};

template<class NodeData>
class List
{

public:

	List()
	{
		m_lpFirst = m_lpLast = NULL;
	}

	~List()
	{
		ListNode<NodeData>* curr = m_lpFirst;
		while (curr != nullptr) {
			curr->Destroy();
			curr = curr->getNext();
		};
	}

	ListNode<NodeData>* GetFirstItem() const
	{
		return m_lpFirst;
	}

	ListNode<NodeData>* GetLastItem() const
	{
		return m_lpLast;
	}

	template <class ParamType>
	NodeData* const Add(const ParamType& Data)
	{
		ListNode<NodeData>* pNewNode = new ListNode<NodeData>(Data);

		if (m_lpFirst == NULL)
			m_lpFirst = pNewNode;

 		if (m_lpLast == NULL)
			m_lpLast = pNewNode;
		else {
			m_lpLast->SetNext(pNewNode);
			m_lpLast = pNewNode;
		}

		return pNewNode->GetNodeDataPointer();
	}

	template <class ParamType>
	NodeData* const AddFront(const ParamType& Data)
	{
		ListNode<NodeData>* pNewNode = new ListNode<NodeData>(Data);

		if (m_lpFirst == NULL)
			m_lpFirst = pNewNode;

		if (m_lpLast == NULL)
			m_lpLast = pNewNode;
		else {
			pNewNode->SetNext(m_lpFirst);
			m_lpFirst = pNewNode;
		}

		return pNewNode->GetNodeDataPointer();
	}

private:

	ListNode<NodeData>* m_lpFirst;
	ListNode<NodeData>* m_lpLast;

};

//- Containers ----------------------------------------------------------------

template <class Attrib>
class AttribData
{
	Attrib*								m_lpElem;
	size								m_Size;

public:

	AttribData()
	{
		m_lpElem = NULL;
		m_Size = 0;
	}

	~AttribData()
	{
		delete [] m_lpElem;
		m_lpElem = NULL;
		m_Size = 0;
	}

	AttribData(const AttribData& rhs)
		: m_lpElem(NULL)
		, m_Size(rhs.Size())
	{
		if(m_Size) {
			m_lpElem = new Attrib[m_Size];
		
			for(index i=0; i<m_Size; i++) {
				Attrib & T = m_lpElem[i];
				T = rhs.GetElem(i);
			}
		}
	}

	AttribData(const Attrib AttribArray[], const size& SizeinBytes)
	{
		m_Size = SizeinBytes  / sizeof(Attrib);
		m_lpElem = new Attrib[m_Size];

		for (index i = 0; i < m_Size; i++) {
			m_lpElem[i] = AttribArray[i];
		}
	}

	AttribData(const size& Size)
		: m_lpElem(NULL)
		, m_Size(0)
	{
		Resize(Size);
	}
	
	AttribData<Attrib>&					operator=(
		AttribData<Attrib>					rhs)
	{
		Swap(m_lpElem, rhs.m_lpElem);
		Swap(m_Size, rhs.m_Size);
		return *this;
	}

	Attrib&								operator[](
		const int							index)
	{
		return m_lpElem[index];
	}

	const Attrib&						operator[](
		const int							index) const
	{
		return m_lpElem[index];
	}

	//- Disk Operations -------------------------------------------------------

	const bool							ReadFromDisk(
		const size							Size,
		FILE*								pFile)
	{
		Resize(Size);
		if(!fread(m_lpElem, sizeof(Attrib), Size, pFile) == Size)
			return false;

		return true;
	}

	const bool							WriteToDisk(
		const unsigned int					Header,
		FILE*								pFile)			const
	{
		fwrite(&Header, sizeof(Header), 1, pFile);
		fwrite(&m_Size, sizeof(m_Size), 1, pFile);
		if(fwrite(m_lpElem, sizeof(Attrib), m_Size, pFile) != m_Size)
			return false;
		
		return true;
	}

	//- Manipulating contents -------------------------------------------------

	AttribData<Attrib>&					Set(
		const AttribData<Attrib>&			rhs)
	{
		AttribData<Attrib> T(rhs);
		Swap(m_lpElem, T.m_lpElem);
		Swap(m_Size, T.m_Size);
		return *this;
	}

	void								Set(
		index								Index,
		const Attrib&						Elem)
	{
		if(Index < m_Size)
			m_lpElem[Index]=Attrib(Elem);
	}

	void								Load(
		const Attrib*						pSrc,
		count								NrItems)
	{
		Clear();
		Resize(NrItems);

		for(index i=0; i<NrItems; i++)
			m_lpElem[i] = pSrc[i];
	}

	Attrib&								SetElem(
		index								i)
	{
		return m_lpElem[i];
	}
	
	void								Add(
		index								Index,
		const Attrib&						Elem)
	{
		if(Index < m_Size)
			m_lpElem[Index]+=Elem;
	}

	//	Resizes the container, copying the contents where possible
	void								Resize(
		size								NewSize)
	{
		if(NewSize != 0) {

			if(m_lpElem == NULL) {
				Attrib* lpNew = new Attrib[NewSize];
				memset(lpNew, 0, NewSize * sizeof(Attrib));

				m_lpElem = lpNew;
				m_Size = NewSize;
			} else if(NewSize > m_Size) {	//	New size is bigger
				Attrib* lpNew = new Attrib[NewSize];
				memset(lpNew, 0, NewSize * sizeof(Attrib));
				memcpy(lpNew, m_lpElem, m_Size * sizeof(Attrib));
				delete [] m_lpElem;

				m_lpElem = lpNew;
				m_Size = NewSize;
			} else if(NewSize < m_Size) {
				Attrib* lpNew = new Attrib[NewSize];
				memset(lpNew, 0, NewSize * sizeof(Attrib));
				memcpy(lpNew, m_lpElem, NewSize * sizeof(Attrib));
				delete [] m_lpElem;

				m_lpElem = lpNew;
				m_Size = NewSize;
			}

		} else {

			delete [] m_lpElem;
			m_Size = 0;
			m_lpElem = NULL;

		}
	}

	void								Clear()
	{
		if(m_lpElem) {
			delete [] m_lpElem;
			m_lpElem = NULL;
			m_Size = 0;
		}
	}
	
	//- Manipulate Contents ---------------------------------------------------

	void								Normalize()
	{
		for(index i=0; i<m_Size; i++)
			m_lpElem[i]=glm::normalize(m_lpElem[i]);
	}

	//	Invalidate float
	void								Max(
		index								Index)
	{
		if(Index < m_Size) {
			uint* pFloat = (uint*)&m_lpElem[Index];
			*pFloat = 0xFFFFFFFF;
		}
	}

	//- Read only -------------------------------------------------------------

	const Attrib*						GetBufferPointer()		const
	{
		return m_lpElem;
	}

	const size&							Size()					const
	{
		return m_Size;
	}

	const size							SizeInBytes()			const
	{
		return m_Size * sizeof(Attrib);
	}

	const Attrib&						GetElem(
		index								 i)					const
	{									
		return m_lpElem[i];				
	}

	Attrib*								GetElemPointer(
		index								 i)					const
	{									
		return &m_lpElem[i];				
	}
};

template <class LibItem>
class Library
{
	LibItem*							m_lpLibItems;
	size								m_Size;

public:

	Library()
	{
		this->m_lpLibItems = NULL;
		this->m_Size = 0;
	}

	~Library()
	{
		if(m_lpLibItems)
		{
			delete [] m_lpLibItems;
			m_lpLibItems = NULL;
			m_Size = 0;
		}
	}

	Library(const Library & rhs)
		:	m_lpLibItems(NULL)
		,	m_Size(rhs.m_Size)
	{
		m_lpLibItems = new LibItem[m_Size];

		for(index i=0; i<m_Size; i++) {
			LibItem & T = m_lpLibItems[i];
			T = rhs.GetItem(i);
		}
	}

	Library(const size& Size)
		:	m_lpLibItems(NULL)
		,	m_Size(0)
	{
		Resize(Size);
	}

	Library<LibItem>&					operator=(
		Library<LibItem>					rhs)
	{
		Swap(m_Size, rhs.m_Size);
		Swap(m_lpLibItems, rhs.m_lpLibItems);
		return *this;
	}

//	LibItem&							operator[](
//		const int							index)
//	{
//		return m_lpLibItems[index];
//	}

	const LibItem&						operator[](
		const int							index)	const
	{
		return m_lpLibItems[index];
	}

	//- Disk operarations ---------------------------------------------------------

	const bool							WriteToDisk(
		const unsigned int					Header,
		FILE*								pFile)
	{
		if(m_Size) {
			fwrite(&Header, sizeof(Header), 1, pFile);
			fwrite(&m_Size, sizeof(m_Size), 1, pFile);
			for(index i=0; i<m_Size; i++)
				if(!m_lpLibItems[i].WriteToDisk(pFile))
					return false;
		}

		return true;
	}

	const bool							ReadFromDisk(
		FILE*								pFile)
	{
		size Size=0;
		fread(&Size, sizeof(Size), 1, pFile);
		Resize(Size);
		for(index i=0; i<m_Size; i++)
			if(!m_lpLibItems[i].ReadFromDisk(pFile))
				return false;

		return true;
	}

	//- Manipulation contents -----------------------------------------------------

	void								Load(
		LibItem*							pSrc,
		size								NrItems)
	{
		if(m_lpLibItems != NULL)
			delete [] m_lpLibItems;

		m_lpLibItems = new LibItem[NrItems];
		m_Size = NrItems;

		for(index i=0; i<NrItems; i++)
			m_lpLibItems[i] = LibItem(pSrc[i]);
	}

	void								Append(
		LibItem*							pSrc,
		count								NrItems)
	{
		const size  Size = m_Size;
		const size& TotalItems = Size + NrItems;

		Resize(TotalItems);

		for(index i=Size; i<TotalItems; i++)
			m_lpLibItems[i] = LibItem(pSrc[i-Size]);
	}
	
	void								Clear()
	{
		delete [] this->m_lpLibItems;
		m_lpLibItems = NULL;
		m_Size = 0;
	}

	void								Resize(
		size								NewSize)
	{
		if(m_lpLibItems == NULL) {
			LibItem* lpNew = new LibItem[NewSize];
			memset(lpNew, 0, NewSize * sizeof(LibItem));

			m_lpLibItems = lpNew;
			m_Size = NewSize;
		} else if(NewSize > m_Size) {	//	New size is bigger
			LibItem* lpNew = new LibItem[NewSize];

			for(uint i=0; i<m_Size; i++)
				lpNew[i] = LibItem(m_lpLibItems[i]);

			delete [] m_lpLibItems;

			m_lpLibItems = lpNew;
			m_Size = NewSize;
		} else if(NewSize < m_Size) {	//	New size is smaller
			LibItem* lpNew = new LibItem[NewSize];

			for(uint i=0; i<NewSize; i++)
				lpNew[i] = LibItem(m_lpLibItems[i]);

			delete [] m_lpLibItems;

			m_lpLibItems = lpNew;
			m_Size = NewSize;
		}
	}

	LibItem*							New()
	{
		Resize(m_Size+1);		
		return &m_lpLibItems[m_Size-1];
	}

	LibItem&							SetItem(
		index								Index)
	{
		return m_lpLibItems[Index];
	}

	void								Set(
		index								i,
		const LibItem&						Item)
	{
		if(i < m_Size)
			m_lpLibItems[i]=Item;
	}

	const size							Size()					const
	{
		return m_Size;
	}

	const size							SizeInBytes()			const
	{
		return m_Size * sizeof(LibItem);
	}

	LibItem*							GetPointer()			const
	{
		return m_lpLibItems;
	}

	const LibItem&						GetItem(
		index								 Index)				const
	{
		return m_lpLibItems[Index];
	}

	//	All Libraryitems must have a char* Name var.
	id									GetItemID(
		const char*							Name)				const
	{
		index i=0;
		for(; i<m_Size; i++)
			if(m_lpLibItems[i].GetName().Equals(Name))
				return i+1;

		//	Not found
		return 0;
	}

	const LibItem&						GetItem(
		const char*							Name)				const
	{
		for(uint i=0; i<m_Size; i++)
			if(!strcmp(m_lpLibItems[i].Name, Name))
				return m_lpLibItems[i];
	}
};
