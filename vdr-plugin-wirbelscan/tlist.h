/*
 * tlist.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */
#ifndef tlist_h
#define tlist_h

/*******************************************************************************
 * TList
 ******************************************************************************/
#include <vector>
#include <algorithm>

// returns true if Item1 should be sorted before Item2 in a TList.
typedef bool (*TListSortCompare)(void* Item1, void* Item2);

template<class T> class TList {
private:
  cMutex mutex;
  void Lock(void)   { mutex.Lock();   }
  void Unlock(void) { mutex.Unlock(); }
protected:
  std::vector<T> v;
public:
  TList() {}
  ~TList() { v.clear(); }

  void Add(T p) {                                        // Add a new item to the list.
     Lock();
     v.push_back(p);
     if (v.size() == v.capacity())
        v.reserve(v.capacity() << 1);
     Unlock();
     }
  int  Capacity() { return v.capacity();              }  // returns max number of items
  void Capacity(int newCap) { v.reserve(newCap);      }  // sets max number of items
  void Clear()    { v.clear();                        }  // Clears the list.
  int  Count()    {
     Lock();
     int n = v.size();
     Unlock();
     return n;
     }  // Current number of items.   
  void Delete(int Index) {                               // Removes items from list.
     Lock(); v.erase(v.begin()+Index); Unlock(); }
  void Exchange(int Index1, int Index2) {                // Exchanges two items
     Lock();
     T p1 = v[Index1];
     T p2 = v[Index2];
     v[Index1] = p2;
     v[Index2] = p1;
     Unlock();
     }
  TList<T> Expand() {                                    // Increases the capacity of the list if needed.                 
     if (v.size() == v.capacity()) {
        unsigned cap = v.capacity();
        if      (cap < 3U) cap += 4;
        else if (cap < 8U) cap += 8;
        else               cap += 16;
        v.resize(cap);
        }
     return *this;
     }
  T First() { return v.front(); }                       // Returns the first non-nil pointer in the list.
  T Last()  { return v.back();  }                       // Returns the last non-nil pointer in the list.

  int IndexOf(T Item) {                                 // Returns the index of a given item.
     for(unsigned pos = 0; pos < v.size(); ++pos) {
        if (v[pos] == Item)
           return pos;
        }
     return -1;
     }
  // Inserts a new pointer in the list at a given position.
  void Insert(int Index, T Item) { v.insert(v.begin() + Index , Item); }
  // Moves a pointer from one position in the list to another.
  void Move(int CurIndex, int NewIndex) {
     if (CurIndex == NewIndex)
        return;     
     if (CurIndex < NewIndex) {
        void* p = v[CurIndex];
        Delete(CurIndex);
        Insert(NewIndex-1, p);
        }
     else {
        void* p = v[CurIndex];
        Delete(CurIndex);
        Insert(NewIndex, p);
        }
     }

  // Provides access to Items (pointers) in the list.
  T& operator[](int const& Index) { return v[Index]; }
  T& Items(int const& Index)      { return v[Index]; }

  // Removes a value from the list & returns it's index before removal
  int Remove(T Item)  {
     for(unsigned pos = 0; pos < v.size(); ++pos) {
        if (v[pos] == Item) {
           v.erase(v.begin()+pos);
           return pos;
           }
        }
     return -1;
     }

  // Sorts the items in the list using a function.
  void Sort(TListSortCompare Compare) { std::sort(v.begin(), v.end(), Compare); }

  // Sorts the items in the list using operator '<'.
  void Sort() { std::sort(v.begin(), v.end()); }

  // Copy the contents of other lists.
  void Assign(TList<T>& from) { v.assign(from.v.begin(), from.v.end()); }

  // Add all items from another list
  void AddList(TList<T>& aList) { v.insert(v.end(),aList.v.begin(),aList.v.end()); }

  // Returns the items in an array.
  T* List() { return v.data(); }

  // Removes NULL pointers from the list and frees unused memory.
  void Pack() { v.shrink_to_fit(); }
};

#endif
