namespace Imagina {
	interface ITask {
		void Cancel();
		bool Finished(); // TODO: Consider changing it to Status()
		bool Terminated();
		void Wait();
		void Release();
	};
}