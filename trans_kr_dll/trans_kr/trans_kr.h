// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, ������ DLL�� ����ϴ� �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// TRANS_KR_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef TRANS_KR_EXPORTS
#define TRANS_KR_API __declspec(dllexport)
#else
#define TRANS_KR_API __declspec(dllimport)
#endif

#if 0
// �� Ŭ������ trans_kr.dll���� ������ ���Դϴ�.
class TRANS_KR_API Ctrans_kr {
public:
	Ctrans_kr(void);
	// TODO: ���⿡ �޼��带 �߰��մϴ�.
};

extern TRANS_KR_API int ntrans_kr;

TRANS_KR_API int fntrans_kr(void);
#endif 

char* WINAPI ko_translate(char* untranslated);
char* WINAPI ko_translate_wo_word(char* untranslated);
void WINAPI ko_ReloadDic();
void WINAPI TransOn(bool on);