#pragma once
#include <atomic>
#include <functional>

namespace YD3D
{
	template<typename T>
	class ystate
	{
	public:
		ystate() :
			m_state(static_cast<T>(0)),
			mHasCallback(false)
		{

		}

		ystate(const T state) :
			m_state(state)
		{

		}

		~ystate()
		{

		}

		T state()
		{
			return static_cast<T>(m_state);
		}

		void set_state(const T state) 
		{
			T old_state = m_state;
			m_state = state;

			if (mHasCallback) 
			{
				mCallback(old_state, state);
			}
		}

		bool is_state(const T state)
		{
			return m_state == state;
		}

		bool has_state_strong(const T state)
		{
			return (m_state & state) == state;
		}

		bool has_state_weak(const T state) 
		{
			return (m_state & state);
		}

		operator T()
		{
			return m_state;
		}

		T operator = (const T state)
		{
			m_state.exchange(state);
			return state;
		}

		void bind_callback(const std::function<void(const T, const T)> &callback)
		{
			mHasCallback = true;
			mCallback = callback;
		}

		void remove_callback() 
		{
			mHasCallback = false;
			mCallback = std::function<void(const T, const T)>();
		}

		T add_state(const T state)
		{
			T old_state;
			T new_state;

			do
			{
				old_state = m_state;
				new_state = static_cast<T>(old_state | state);
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback) 
			{
				mCallback(old_state, new_state);
			}

			return new_state;
		}

		T remove_state(const T state)
		{
			T old_state;
			T new_state;

			do
			{
				old_state = m_state;
				new_state = static_cast<T>(old_state & (~state));
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return new_state;
		}

		T transfer_state(const T remove_state, const T add_state)
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				new_state = static_cast<T>(old_state & (~remove_state));
				new_state = static_cast<T>(new_state | add_state);
			}
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return new_state;
		}

		bool compare_and_set(const T compare_state, const T set_state)
		{
			T old_state = compare_state;
			T new_state = set_state;

			if (m_state.compare_exchange_strong(compare_state, set_state)) 
			{
				if (mHasCallback)
				{
					mCallback(old_state, new_state);
				}
				return true;
			}
			else 
			{
				return false;
			}
		}

		bool compare_and_add(const T compare_state, const T state) 
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (old_state != compare_state)
				{
					return false;
				}
				new_state = static_cast<T>(old_state | state);
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

		bool compare_and_remove(const T compare_state, const T state) 
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (old_state != compare_state)
				{
					return false;
				}
				new_state = static_cast<T>((old_state & (~state)));
			}
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

		bool unequal_and_set(const T compare_state, const T state)
		{
			T old_state;
			do 
			{
				old_state = m_state;
				if (old_state == compare_state) 
				{
					return false;
				}
			} 
			while (!m_state.compare_exchange_strong(old_state, state));
			
			if (mHasCallback)
			{
				mCallback(old_state, state);
			}

			return true;
		}

		bool unequal_and_add(const T compare_state, const T state)
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (old_state == compare_state)
				{
					return false;
				}
				new_state = static_cast<T>(old_state | state);
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

		bool unequal_and_remove(const T compare_state, const T state)
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (old_state == compare_state)
				{
					return false;
				}
				new_state = static_cast<T>(old_state | &(~state));
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

		bool has_and_set(const T compare_state, const T state)
		{
			T old_state;
			do
			{
				old_state = m_state;
				if (!(old_state & compare_state))
				{
					return false;
				}
			} 
			while (!m_state.compare_exchange_strong(old_state, state));

			if (mHasCallback)
			{
				mCallback(old_state, state);
			}

			return true;
		}

		bool has_and_add(const T compare_state, const T state)
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (!(old_state & compare_state))
				{
					return false;
				}
				new_state = static_cast<T>(old_state | state);
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

		bool has_and_transfer(const T compare_state, const T add_state, const T remove_state)
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (!(old_state & compare_state))
				{
					return false;
				}
				new_state = static_cast<T>(old_state & (~remove_state));
				new_state = static_cast<T>(new_state | add_state);
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

		bool has_and_remove(const T compare_state, const T state)
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (!(old_state & compare_state))
				{
					return false;
				}
				new_state = static_cast<T>(old_state & (~state));
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

		bool null_and_set(const T compare_state, const T state)
		{
			T old_state;
			do
			{
				old_state = m_state;
				if (old_state & compare_state)
				{
					return false;
				}
			} 
			while (!m_state.compare_exchange_strong(old_state, state));

			if (mHasCallback)
			{
				mCallback(old_state, state);
			}

			return true;
		}

		bool null_and_add(const T compare_state, const T state)
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (old_state & compare_state)
				{
					return false;
				}
				new_state = static_cast<T>(old_state | state);
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

		bool null_and_remove(const T compare_state, const T state)
		{
			T old_state;
			T new_state;
			do
			{
				old_state = m_state;
				if (old_state & compare_state)
				{
					return false;
				}
				new_state = static_cast<T>(old_state & (~state));
			} 
			while (!m_state.compare_exchange_strong(old_state, new_state));

			if (mHasCallback)
			{
				mCallback(old_state, new_state);
			}

			return true;
		}

	private:
		std::atomic<T>							m_state;
		bool									mHasCallback;
		std::function<void(const T, const T)>	mCallback;
	};



};
