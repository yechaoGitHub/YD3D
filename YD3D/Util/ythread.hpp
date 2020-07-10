#include "YD3D_Header.h"

template<typename TReturn = void>
class ythread
{
public:
	ythread() :
		m_running(false),
		m_run_loop(false)
	{

	}

	~ythread()
	{
		if (m_run_loop) 
		{
			quit();
			wait_for_quit();
		}
	}

	template<typename TFunction, typename ...TParam>
	bool run_once(TFunction func, TParam&&... args)
	{
		if (running())
		{
			return false;
		}

		m_run_loop = false;
		m_function = std::bind(func, args...);
		m_thread = std::thread(&ythread::thread_function, this);
		return true;
	}

	template<typename TFunction, typename ...TParam>
	bool run_loop(TFunction func, TParam&&... args)
	{
		if (running())
		{
			return false;
		}

		m_run_loop = true;
		m_function = std::bind(func, args...);
		m_thread = std::thread(&ythread::thread_function, this);
		return true;
	}

	bool running()
	{
		return m_running;
	}

	bool quit()
	{
		m_run_loop = false;
		return true;
	}

	bool wait_for_quit()
	{
		m_thread.join();
		return true;
	}

	TReturn	return_value()
	{
		return m_return;
	}

protected:

	void thread_function()
	{
		m_running = true;

		do
		{
			m_return = std::move(m_function());
		} 
		while (m_run_loop);

		m_running = false;
	}

	std::thread						m_thread;
	std::function<TReturn(void)>	m_function;
	TReturn							m_return;
	bool							m_running;
	bool							m_run_loop;
};

template<>
class ythread<void>
{
public:
	ythread() :
		m_running(false),
		m_run_loop(false)
	{

	}

	~ythread()
	{
		if (m_run_loop)
		{
			quit();
			wait_for_quit();
		}
	}

	template<typename TFunction, typename ...TParam>
	bool run_once(TFunction&& func, TParam&&... args)
	{
		if (running())
		{
			return false;
		}

		m_run_loop = false;
		m_function = std::bind(std::forward<TFunction>(func), std::forward<TParam>(args)...);
		m_thread = std::thread(&ythread::ThreadFunction, this);
		return true;
	}

	template<typename TFunction, typename ...TParam>
	bool run_loop(TFunction&& func, TParam&&... args)
	{
		if (running())
		{
			return false;
		}

		m_run_loop = true;
		m_function = std::bind(std::forward<TFunction>(func), std::forward<TParam>(args)...);
		m_thread = std::thread(&ythread::ThreadFunction, this);
		return true;
	}

	bool running()
	{
		return m_running;
	}

	bool quit()
	{
		m_run_loop = false;
		return true;
	}

	bool wait_for_quit()
	{
		m_thread.join();
		return true;
	}

protected:

	void ThreadFunction()
	{
		m_running = true;

		do
		{
			m_function();
		} 
		while (m_run_loop);

		m_running = false;
	}

	std::thread						m_thread;
	std::function<void(void)>		m_function;
	volatile bool					m_running;
	volatile bool					m_run_loop;
};

