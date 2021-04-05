
#ifndef __IO_LIBRARY__
#define __IO_LIBRARY__

//MEMO
//�f�B���N�g���֌W�̊֐��́A�p�X�̕�����̍Ō�́u/�v�̗L���Ɋ֌W�Ȃ����삵�܂�


//�o�b�t�@��4�̔{�����傫����Α傫���قǑ����R�s�[���I���͂��ł��B���Ԃ�
#define COPY_BUFFERSIZE			512


enum {
	PATH_LAST_IS_STR = 0,
	PATH_LAST_IS_DIR = 1
};


/* ----------------------------------------------------
 * �t�@�C�����폜����
 *
 * @param	filename : �t�@�C����
 *
 * @retval	���� : 0
 *			���s : -1
*/
int Remove(const char *filename);

/* ----------------------------------------------------
 * �f�B���N�g�����폜����
 * ���̊K�w���܂߂Ă��ׂč폜���܂�
 *
 * @param	path : �f�B���N�g����
 *
 * @retval	���� : 0
 *			���s : -1
*/
int Remove_Directory(const char *path);

/* ----------------------------------------------------
 * �t�@�C�����R�s�[����
 *
 * @param	in : �R�s�[���t�@�C����
 *			out : �R�s�[��t�@�C����
 *
 * @retval	���� : 0
 *			���s : -1
*/
int Copy(const char *in, const char *out);

/* ----------------------------------------------------
 * �f�B���N�g�����R�s�[����
 *
 * @param	in : �R�s�[���f�B���N�g��
 *			out : �R�s�[��f�B���N�g��
 *
 * @retval	���� : 0
 *			���s : -1
*/
int Copy_Directory(const char* in, const char *out);

/* ----------------------------------------------------
 * �p�X�̍Ōオ '/' ���ǂ������`�F�b�N����
 *
 * @retval	0 (PATH_LAST_IS_STR) : �Ō�͕�����
 *			1 (PATH_LAST_IS_DIR) : �Ō�� '/'
*/
int CheckPathType(const char* path);

/* ----------------------------------------------------
 * flash0�̃v���e�N�g����������
 *
 * @retval	���� : 0
 *			���s : -1
*/
int FlashWriteEnable(void);

/* ----------------------------------------------------
 * flash0���v���e�N�g����
 *
 * @retval	���� : 0
 *			���s : -1
*/
int FlashProtect(void);

int GetDirFileCount(const char *dirname);
int Log(const char *filename, const char *format,...);


#endif