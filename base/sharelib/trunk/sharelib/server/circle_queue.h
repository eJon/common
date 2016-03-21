#ifndef SHARELIB_CIRCLE_QUEUE_H_
#define SHARELIB_CIRCLE_QUEUE_H_
SHARELIB_BS;
struct STURCT_NODE{
    void* m_pData;
    STURCT_NODE * m_pNext;
};
template <class T>
class CQueue{
public:
    inline CQueue(){
        m_nNodeCount = 0;
        m_pHead = NULL;
        m_pTail = NULL;

        m_pCacheList = NULL;
        m_nCacheCount = 0;
        m_nMaxCacheSize = 5;
    }
    inline ~CQueue(){
        STURCT_NODE * lpNode=NULL;
        while (m_pHead != NULL){
            lpNode = m_pHead->m_pNext;
            delete (T*)m_pHead->m_pData;
            delete m_pHead;
            m_pHead =lpNode;
        }
        m_nNodeCount = 0;

        while (m_pCacheList != NULL){
            lpNode = m_pCacheList->m_pNext;			
            delete m_pCacheList;
            m_pCacheList = lpNode;
        }
        m_nCacheCount = 0;        
    };

    inline int AddTail(T * apValue){
        STURCT_NODE * lpNode= MallocNode();
        lpNode->m_pNext = NULL;
        lpNode->m_pData = apValue;
        if (m_pTail == NULL){
            m_pHead = lpNode;
            m_pTail = lpNode;
        }else{
            m_pTail->m_pNext =lpNode;
            m_pTail = lpNode;
        }
        m_nNodeCount++;
        return m_nNodeCount;
    };

    inline T * GetHead(){
        if (m_pHead == NULL)
            return NULL;
        return (T*)m_pHead->m_pData;
    };

    inline T * PopHead(){
        if (m_pHead == NULL){
            return NULL;
        }else{
            T *lpStru = (T*)m_pHead->m_pData;
            STURCT_NODE * lpNode=NULL;
            lpNode = m_pHead->m_pNext;
            FreeNode(m_pHead);
            m_pHead = lpNode;

            if (m_pHead == NULL)
                m_pTail= NULL;

            m_nNodeCount--;
            return lpStru;
        }
    };
    inline void ClearAll() {
        STURCT_NODE * lpNode=NULL;
        while (m_pHead != NULL){
            lpNode = m_pHead->m_pNext;
            delete (T*)m_pHead->m_pData;
            FreeNode(m_pHead);
            m_pHead = lpNode;
            if (m_pHead == NULL){
                m_pTail= NULL;
            }
        }
        m_nNodeCount = 0;
    }
    inline int GetCount(){return m_nNodeCount;};

    inline void SetCacheSize(int nCacheSize){
        m_nMaxCacheSize = nCacheSize;
    }
    inline STURCT_NODE *MallocNode(){
        if (m_pCacheList != NULL){
            STURCT_NODE * lpNode = m_pCacheList;
            m_pCacheList = m_pCacheList->m_pNext;
            m_nCacheCount--;
            return lpNode; 
        }else{
            return new STURCT_NODE();
        }
    }

    inline void FreeNode(STURCT_NODE *&pNode) {
        if (m_nCacheCount < m_nMaxCacheSize){
            pNode->m_pNext = m_pCacheList;
            m_pCacheList = pNode;
            m_nCacheCount++;
        }else{
            delete pNode;
        }
        pNode = NULL;
    }
private:
    int m_nNodeCount;
    STURCT_NODE *m_pHead;
    STURCT_NODE *m_pTail;
    STURCT_NODE *m_pCacheList;
    int m_nCacheCount;
    int m_nMaxCacheSize;
};
SHARELIB_ES;
#endif 

