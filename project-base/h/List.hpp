#ifndef PROJECT_BASE_LIST_HPP
#define PROJECT_BASE_LIST_HPP

template<typename T> class List{

private:

    struct Elem{
        T* data;
        Elem* next;
        Elem(T *data, Elem *next) : data(data), next(next) {}
    };

    Elem* listHead = nullptr;
    Elem* listTail = nullptr;

public:

    void addToFront(T* data){
        Elem* newNode = new Elem(data, listHead);
        listHead = newNode;
        if(!listTail)
            listTail = listHead;
    }

    T* removeFront(){
        if (!listHead)
            return nullptr;
        Elem *removedNode = listHead;
        T *data = removedNode->data;
        listHead = listHead->next;
        if (!listHead)
            listTail = listHead;
        delete removedNode;
        return data;
    }

    T* peekFront(){
        if(listHead)
            return listHead->data;
        return nullptr;
    }

    void addToBack(T* data){
        Elem* newNode = new Elem(data, nullptr);
        if(listTail)
            listTail->next = newNode;
        listTail = newNode;
        if(!listHead)
            listHead = listTail;
    }

    T* removeBack(){
        if(!listTail)
            return nullptr;
        Elem* removedNode = listTail;
        T* data = removedNode->data;
        Elem* prev = nullptr;
        Elem* curr = listHead;
        for (; curr != listTail; curr = curr->next) {
            prev = curr;
        }
        listTail = prev;
        if(listTail)
            listTail->next = nullptr;
        else
            listHead = listTail;

        delete removedNode;
        return data;
    }

    T* peekBack(){
        if(listTail)
            return listTail->data;
        return nullptr;
    }

};

#endif //PROJECT_BASE_LIST_HPP