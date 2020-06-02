/********************************************************************************
 * MIT License
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Copyright (c) 2020 Sarayu Nhookeaw
 *
 ********************************************************************************/

#ifndef CM_DEFER_H
#define CM_DEFER_H

/////////////////////////////////////////////////////////////////////////////////
//
// Defer statement
// Akin to D's SCOPE_EXIT or
// similar to Go's defer but scope-based
//
// NOTE: C++11 (and above) only!
//
/////////////////////////////////////////////////////////////////////////////////

#if !defined(CM_NO_DEFER) && defined(__cplusplus) && ((defined(_MSC_VER) && _MSC_VER >= 1400) || (__cplusplus >= 201103L))
extern "C++" {
	// NOTE(bill): Stupid fucking templates
	template <typename T> 
    struct cmRemoveReference       { typedef T Type; };
	template <typename T> 
    struct cmRemoveReference<T &>  { typedef T Type; };
	template <typename T> 
    struct cmRemoveReference<T &&> { typedef T Type; };

	/// NOTE(bill): "Move" semantics - invented because the C++ committee are idiots (as a collective not as indiviuals (well a least some aren't))
	template <typename T> 
    inline T &&cm_forward(typename cmRemoveReference<T>::Type &t)  { 
        return static_cast<T &&>(t); 
    }

	template <typename T> 
    inline T &&cm_forward(typename cmRemoveReference<T>::Type &&t) { 
        return static_cast<T &&>(t); 
    }

	template <typename T> 
    inline T &&cm_move (T &&t)  { 
        return static_cast<typename cmRemoveReference<T>::Type &&>(t); 
    }

	template <typename F>
	struct cmprivDefer {
		F f;
		cmprivDefer(F &&f) : f(cm_forward<F>(f)) {}
		~cmprivDefer() { f(); }
	};

	template <typename F> 
    cmprivDefer<F> cm__defer_func(F &&f) { 
        return cmprivDefer<F>(cm_forward<F>(f)); 
    }

	#define CM_DEFER_1(x, y) x##y
	#define CM_DEFER_2(x, y) CM_DEFER_1(x, y)
	#define CM_DEFER_3(x)    CM_DEFER_2(x, __COUNTER__)
	#define defer(code)      auto CM_DEFER_3(_defer_) = cm__defer_func([&]()->void{code;})
}

// Example
#if 0
	cmMutex m;
	cm_mutex_init(&m);
	{
		cm_mutex_lock(&m);
		defer (cm_mutex_unlock(&m));

		...
	}
#endif

#endif //CM_NO_DEFER

#endif //CM_DEFER_H