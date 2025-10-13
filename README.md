# 엘소드 유틸

이 저장소는 엘소드(Elsword) 게임 플레이에 도움이 되는 유틸리티를 제공합니다.

## 엘소드 공명도 계산기

초상 및 마그마의 숨결에서 획득할 수 있는 경험치를 계산할 수 있습니다.

**[➡️ 공명도 계산기](https://ich-mag-dich.github.io/elsword/)**

## 엘소드 해상도 변경

엘소드 게임의 해상도를 인게임에서 지원하지 않는 값(1920:810)으로 변경할 수 있는 프로그램입니다.

### 실행 파일 다운로드

**[➡️ 최신 릴리스 다운로드](https://github.com/ich-mag-dich/elsword/releases)**

페이지에서 미리 컴파일된 `ElswordResizer.exe` 파일을 받으실 수 있습니다.

### (선택) 직접 빌드하기

`g++` 컴파일러(예: MinGW)가 설치되어 있다면, 아래 명령어로 직접 빌드할 수 있습니다.

```sh
g++ elsword_window_resizer/elsword_resizer.cpp -o ElswordResizer.exe -luser32 -lgdi32 -mwindows -municode -static-libgcc -static-libstdc++
```

파일을 관리자 권한으로 실행해주세요

### 사용 방법

1. 다운로드 받았거나 직접 빌드한 `ElswordResizer.exe` 파일을 준비합니다.
2. 파일을 마우스 오른쪽 버튼으로 클릭한 후, **관리자 권한으로 실행**을 선택해주세요.

> ⚠️ 주의: 이 프로그램은 게임 윈도우를 직접 제어하므로, 올바른 작동을 위해 관리자 권한이 반드시 필요합니다.

### 스크린샷

<img width="356" height="418" alt="image" src="https://github.com/user-attachments/assets/1371fd32-da58-4068-b769-8434a1044771" />
<img width="356" height="418" alt="image" src="https://github.com/user-attachments/assets/8fc58763-9779-4a1d-997a-410d2143357f" />
