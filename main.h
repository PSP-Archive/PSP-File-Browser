static int running = 1;
SceCtrlData pad;

int i,		//���[�v�J�E���^
	flag,	//�{�^���t���O
	fname,	//�t�@�C���ԍ�
	status,	//�X���b�h�E�������[�X�e�B�b�N���
	length,	//�t�@�C���Ȃǂ̒���
	page,	//�\�����̃y�[�W
	state,		//���j���[��Ԕ��f
	sub_state,	//�\���̏�Ԕ��f
	sx,		//�摜�\��x���W
	sy,		//�摜�\��y���W
	page_count,	//���݂̃y�[�W
	page_max,	//�y�[�W��
	sub_fname,	//�t�@�C���ԍ�
	sub_length,	//�t�@�C���Ȃǂ̒���
	sub_page,	//�\�����̃y�[�W
	sub_page_count,	//���݂̃y�[�W
	sub_page_max,	//�y�[�W��
			//sub_xxx�̓R�s�[�E�؂��莞�̃t�@�C�����X�g���Ɏg�p
	action;		//System Settings�Ŏg�p

intraFont* ltn[8];	//���e���t�H���g
intraFont* jpn0;	//���{��t�H���g

enum colors {
	RED		= 0xFF0000FF,
	LITERED		= 0xFF3333FF,
	GREEN		= 0xFF00FF00,
	BLUE		= 0xFFFF0000,
	LITEBLUE	= 0xFFFF3333,
	WHITE	 	= 0xFFFFFFFF,
	HIGHLITEGRAY	= 0xFFEFEFEF,
	LITEGRAY	= 0xFFBFBFBF,
	GRAY		= 0xFF7F7F7F,
	DARKGRAY	= 0xFF3F3F3F,
	BLACK		= 0xFF000000,
};

typedef struct DiskInfo {
	unsigned long cluster_max;	//���N���X�^��
	unsigned long cluster_free;	//�󂫃N���X�^��
	int sector_max;			//����̂����󂫃N���X�^��
	int sector_size;		//�Z�N�^������̃o�C�g��
	int sector_count;		//�N���X�^������̃Z�N�^��
	u64 smax;			//���e��
	u64 sfree;			//�󂫗e��
	u64 sused;			//�g�p�e��
}DiskInfo;

// �������[�X�e�B�b�N����
int MSCallback(int arg1, int arg2, void *common);
int MSCallbackThread(SceSize args, void *argp);
int exit_callback(int arg1, int arg2, void *common);
// �I������
int CallbackThread(SceSize args, void *argp);
int SetupCallbacks(void);
//���̑�
int loadFonts(SceSize args, void *argp);
int drawImage(int *fname, int *entry, SceIoDirent *dir, int mode);
int playMP3(int *fname, int *entry, SceIoDirent *dir, int mode, Image* Background);
int SystemSet(int action);
int SystemSetting();
int getDiskInfo(DiskInfo *info);
int printDiskInfo();
float intraFontPrintJP(int x, int y, char *text);
float intraFontPrintfJP(int x, int y, char *text, ...);
int get_registry_value(const char *dir, const char *name, u32 *val);
int set_registry_value(const char *dir, const char *name, u32 val);

#define ERROR_NO_BEFORE_IMG	0x80000001
#define ERROR_NO_AFTER_IMG	0x80000002
#define ERROR_UNSUPPORTED	0x80000004
#define ERROR_NO_BEFORE_MP3	0x80000008
#define ERROR_NO_AFTER_MP3	0x8000000F
#define IS_PNG				(dir[*fname].d_stat.st_attr & FIO_SO_IFREG && tmp != NULL && stricmp(tmp,".png") == 0)
#define IS_MP3				(dir[*fname].d_stat.st_attr & FIO_SO_IFREG && tmp != NULL && stricmp(tmp,".mp3") == 0)