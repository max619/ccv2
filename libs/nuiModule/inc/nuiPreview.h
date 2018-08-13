#ifndef NUI_PREVIEW
#define NUI_PREVIEW

#include <mutex>

struct imageData
{
	void* data;
	int channels;
	int width;
	int height;

	size_t elementSize;
};

class nuiPreview
{
public:
	nuiPreview();
	~nuiPreview();
	bool isAttached();
	void attachTo(int socket);
	void detach();
	
	void sendData(imageData& data);
	

private:
	int sock;
	bool attached;
	std::mutex mtx;
};


#endif // !NUI_PREVIEW
