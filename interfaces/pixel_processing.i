namespace Imagina {
	struct PixelDataInfo;

	interface IPixelProcessor {
		void SetInput(const PixelDataInfo *info);
		const PixelDataInfo *GetOutputInfo();

		void Process(void *output, void *input) const;
	};
}