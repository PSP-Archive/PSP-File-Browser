
#ifndef __IO_LIBRARY__
#define __IO_LIBRARY__

//MEMO
//ディレクトリ関係の関数は、パスの文字列の最後の「/」の有無に関係なく動作します


//バッファが4の倍数且つ大きければ大きいほど早くコピーが終わるはずです。たぶん
#define COPY_BUFFERSIZE			512


enum {
	PATH_LAST_IS_STR = 0,
	PATH_LAST_IS_DIR = 1
};


/* ----------------------------------------------------
 * ファイルを削除する
 *
 * @param	filename : ファイル名
 *
 * @retval	成功 : 0
 *			失敗 : -1
*/
int Remove(const char *filename);

/* ----------------------------------------------------
 * ディレクトリを削除する
 * 下の階層も含めてすべて削除します
 *
 * @param	path : ディレクトリ名
 *
 * @retval	成功 : 0
 *			失敗 : -1
*/
int Remove_Directory(const char *path);

/* ----------------------------------------------------
 * ファイルをコピーする
 *
 * @param	in : コピー元ファイル名
 *			out : コピー先ファイル名
 *
 * @retval	成功 : 0
 *			失敗 : -1
*/
int Copy(const char *in, const char *out);

/* ----------------------------------------------------
 * ディレクトリをコピーする
 *
 * @param	in : コピー元ディレクトリ
 *			out : コピー先ディレクトリ
 *
 * @retval	成功 : 0
 *			失敗 : -1
*/
int Copy_Directory(const char* in, const char *out);

/* ----------------------------------------------------
 * パスの最後が '/' かどうかをチェックする
 *
 * @retval	0 (PATH_LAST_IS_STR) : 最後は文字列
 *			1 (PATH_LAST_IS_DIR) : 最後は '/'
*/
int CheckPathType(const char* path);

/* ----------------------------------------------------
 * flash0のプロテクトを解除する
 *
 * @retval	成功 : 0
 *			失敗 : -1
*/
int FlashWriteEnable(void);

/* ----------------------------------------------------
 * flash0をプロテクトする
 *
 * @retval	成功 : 0
 *			失敗 : -1
*/
int FlashProtect(void);

int GetDirFileCount(const char *dirname);
int Log(const char *filename, const char *format,...);


#endif