/*
 *      Copyright (C) 2015 Jean-Luc Barriere
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301 USA
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef MYTHLOCKED_H
#define	MYTHLOCKED_H

namespace Myth
{
  /**
   * This implements a "guard" pattern
   */
  class LockGuard
  {
  public:
    struct Lockable;
    /**
     * Initialize a guard which hold the lock. The lock will be released by the
     * destructor.
     * @param lock The pointer to lockable object
     */
    LockGuard(Lockable* lock);
    ~LockGuard();

    /**
     * Create a new lockable object. The allocated resource must be freed by
     * calling DestroyLock.
     * @return The pointer to the new lockable object
     */
    static Lockable* CreateLock();
    /**
     * Destroy lockable object previously allocated with CreateLock.
     * @param lock The pointer to lockable object
     */
    static void DestroyLock(Lockable* lock);
    /**
     * Return once the lock is held and recursive count has been incremented.
     * @param lock The pointer to lockable object
     */
    static void Lock(Lockable* lock);
    /**
     * Return once the lock is released or recursive count has been decremented.
     * @param lock The pointer to lockable object
     */
    static void Unlock(Lockable* lock);
    /**
     * Return once recursive count has been cleared and the lock is released.
     * @param lock
     */
    static void ClearLock(Lockable* lock);

  private:
    Lockable* m_lock;
  };

  template<typename T>
  class Locked
  {
  public:
    Locked(const T& val)
    : m_val(val)
    , m_lock(LockGuard::CreateLock()) {}

    ~Locked()
    {
      LockGuard::DestroyLock(m_lock);
    }

    T Load()
    {
      LockGuard g(m_lock);
      return m_val; // return copy
    }

    const T& Store(const T& newval)
    {
      LockGuard g(m_lock);
      m_val = newval;
      return newval; // return input
    }

    class value
    {
    public:
      value(T& val, LockGuard::Lockable*& lock) : m_val(val), m_g(lock) {}
      T& operator()() const { return m_val; }
    private:
      T& m_val;
      LockGuard m_g;
    };

    value Get()
    {
      return value(m_val, m_lock);
    }

  protected:
    T m_val;
    LockGuard::Lockable* m_lock;

    // Prevent copy
    Locked(const Locked<T>& other);
    Locked<T>& operator=(const Locked<T>& other);
  };

  template <typename T>
  class LockedNumber : public Locked<T>
  {
  public:
    LockedNumber(T val)
    : Locked<T>(val) {}

    T Add(T amount)
    {
      LockGuard g(Locked<T>::m_lock);
      return Locked<T>::m_val += amount;
    }

    T operator+=(T amount)
    {
      return Add(amount);
    }

    T Sub(T amount)
    {
      LockGuard g(Locked<T>::m_lock);
      return Locked<T>::m_val -= amount;
    }

    T operator-=(T amount)
    {
      return Sub(amount);
    }
  };

}

#endif	/* MYTHLOCKED_H */
