namespace Imagina {
	interface ITask {
		void Cancel();
		bool Finished();
		bool Terminated();
		void Wait();
		void Release();
	};
}