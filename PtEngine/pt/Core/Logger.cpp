#include "Logger.hpp"
#include <thread>
#include <filesystem>
#include <ctime>
#include <sys/timeb.h>

namespace pt {

	int fileClose(FILE* handle) {
		return handle ? fclose(handle) : 0;
	}

	Logger::Logger() : mLogFile(nullptr, &fileClose), mStartTime(0.0f) {
	}

	bool Logger::initialize() {
		if (!std::filesystem::is_directory("./logs")) {
			if (!std::filesystem::create_directory("./logs")) {
				fmt::print("Failed to create a logs directory");
				return false;
			}
		}

		std::string fullPath;
		time_t now = time(nullptr);
		tm localTime;
		if (localtime_s(&localTime, &now) == 0) {
			fullPath = fmt::format("./logs/PtEngineLog_{:0>2}_{:0>2}_{:0>4}__{:0>2}_{:0>2}_{:0>3}.txt",
				localTime.tm_mday, localTime.tm_mon + 1, localTime.tm_year + 1900,
				localTime.tm_hour, localTime.tm_min, localTime.tm_sec
			);
		}
		else {
			fullPath = fmt::format("./logs/PtEngineLog_{}.txt", now);
		}

		FILE* logFile;
		if (fopen_s(&logFile, fullPath.data(), "w") != 0) {
			fmt::print("Failed to initialize Logger to writing a log file: log filename is empty");
			return false;
		}

		mLogFile = std::unique_ptr<FILE, decltype(&fileClose)>(logFile, &fileClose);
		mStartTime = mGetTime();
		return true;
	}

	void Logger::write(LogLevel level, std::string_view message, const char* file, const char* funcName, int line) {
		float elapsedTime = mGetTime() - mStartTime;
		uint32_t threadId = *reinterpret_cast<uint32_t*>(&std::this_thread::get_id());

		std::string text;
		const char* levelStr = "";

		switch (level) {
		case LogLevel::Error: levelStr = "ERROR  "; break;
			text = fmt::format("{:.3} ({}) [{}]: {}\n", elapsedTime, threadId, levelStr, message);
		case LogLevel::Warning: levelStr = "WARNING"; break;
			text = fmt::format("{:.3} ({}) [{}]: {}\n", elapsedTime, threadId, levelStr, message);
		case LogLevel::Info: levelStr = "INFO   "; break;
			text = fmt::format("{:.3} ({}) [{}]: {}\n", elapsedTime, threadId, levelStr, message);
		case LogLevel::Trace: levelStr = "TRACE  "; break;
			text = fmt::format("{:.3} ({}) [{}]: {}\n", elapsedTime, threadId, levelStr, message);
		case LogLevel::FatalError:
			text = fmt::format(
				"\n######################################\n{:.3} ({}) [FATAL ERROR]:\nFILE: {}\nFUNCTION: {}\nLINE: {}\nREASON: {}\n######################################\n\n",
				elapsedTime, threadId, file, funcName, line, message
			);
		case LogLevel::AssertionFailed:
			std::string text = fmt::format(
				"\n######################################\n{:.3} ({}) [ASSERTION FAILED]:\nFILE: {}\nFUNCTION: {}\nLINE: {}\nExpression: {}\n######################################\n\n",
				elapsedTime, threadId, file, funcName, line, message
			);
		}

		std::unique_lock lock(mWriteMutex);
		if (mLogFile) {
			fmt::print(mLogFile.get(), text);
		}
		fmt::print(text);
	}

	void Logger::write(LogLevel level, std::string_view message) {
		float elapsedTime = mGetTime() - mStartTime;
		uint32_t threadId = *reinterpret_cast<uint32_t*>(&std::this_thread::get_id());

		std::string text;
		const char* levelStr = "";

		switch (level) {
		case LogLevel::Error: levelStr = "ERROR  "; break;
		case LogLevel::Warning: levelStr = "WARNING"; break;
		case LogLevel::Info: levelStr = "INFO   "; break;
		case LogLevel::Trace: levelStr = "TRACE  "; break;
		}
		text = fmt::format("{:.3} ({}) [{}]: {}\n", elapsedTime, threadId, levelStr, message);

		std::unique_lock lock(mWriteMutex);
		if (mLogFile) {
			fmt::print(mLogFile.get(), text);
		}
		fmt::print(text);
	}

	/*bool Logger::checkAssert(bool expr, std::string_view message, const char *file, const char *funcName, int line) {
		if (!expr) {
			float elapsedTime = mGetTime() - mStartTime;
			uint32_t threadId = *reinterpret_cast<uint32_t*>(&std::this_thread::get_id());
			std::string text = fmt::format(
				"\n######################################\n{:.3} ({}) [ASSERTION FAILED]:\nFILE: {}\nFUNCTION: {}\nLINE: {}\nExpression: {}\n######################################\n\n",
				elapsedTime, threadId, file, funcName, line, message
			);

			std::unique_lock lock(mWriteMutex);
			if (mLogFile) {
				fmt::print(mLogFile.get(), text);
			}
			fmt::print(text);
			return false;
		}
		return true;
	}*/

	float Logger::mGetTime() const {
		_timeb timeb;
		if (_ftime_s(&timeb) == 0) {
			return static_cast<float>(timeb.time) + static_cast<float>(timeb.millitm) / 1000.0f;
		}
		else {
			return 0.0f;
		}
	}

}