static int running = 1;
SceCtrlData pad;

int i,		//ループカウンタ
	flag,	//ボタンフラグ
	fname,	//ファイル番号
	status,	//スレッド・メモリースティック状態
	length,	//ファイルなどの長さ
	page,	//表示中のページ
	state,		//メニュー状態判断
	sub_state,	//予備の状態判断
	sx,		//画像表示x座標
	sy,		//画像表示y座標
	page_count,	//現在のページ
	page_max,	//ページ数
	sub_fname,	//ファイル番号
	sub_length,	//ファイルなどの長さ
	sub_page,	//表示中のページ
	sub_page_count,	//現在のページ
	sub_page_max,	//ページ数
			//sub_xxxはコピー・切り取り時のファイルリスト時に使用
	action;		//System Settingsで使用

intraFont* ltn[8];	//ラテンフォント
intraFont* jpn0;	//日本語フォント

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
	unsigned long cluster_max;	//総クラスタ数
	unsigned long cluster_free;	//空きクラスタ数
	int sector_max;			//きりのいい空きクラスタ数
	int sector_size;		//セクタあたりのバイト数
	int sector_count;		//クラスタあたりのセクタ数
	u64 smax;			//総容量
	u64 sfree;			//空き容量
	u64 sused;			//使用容量
}DiskInfo;

// メモリースティック操作
int MSCallback(int arg1, int arg2, void *common);
int MSCallbackThread(SceSize args, void *argp);
int exit_callback(int arg1, int arg2, void *common);
// 終了処理
int CallbackThread(SceSize args, void *argp);
int SetupCallbacks(void);
//その他
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