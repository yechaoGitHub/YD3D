#pragma once
#include <atomic>

namespace YD3D
{
	template<typename T>
	class ystate
	{
	public:
		ystate() :
			m_state(static_cast<T>(0))
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
			m_state = state;
		}

		bool is_state(const T state)
		{
			return m_state == state;
		}

		bool has_state(const T state)
		{
			return (m_state & state) == state;
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

		T add_state(const T state)
		{
			T old_state;
			T new_state;

			do
			{
				old_state = m_state;
				new_state = static_cast<T>(old_state | state);
			} while (!m_state.compare_exchange_strong(old_state, new_state));

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
			} while (!m_state.compare_exchange_strong(old_state, new_state));

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
				new_state |= static_cast<T>(add_state);
			}
			while (m_state.compare_exchange_strong(old_state, new_state));

			return new_state;
		}

		inline bool compare_and_set(const T compare_state, const T set_state)
		{
			return m_state.compare_exchange_strong(compare_state, set_state);
		}

	private:
		std::atomic<T> m_state;
	};
};
