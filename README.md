# unix-recovery-program

## 🚀구현
1. 모든 입력은 문자열로 받도록 처리했어요.
2. `시작 인덱스(start_addr)`와 `끝 인덱스(end_addr)`, `입력 값`을 메모리 매핑을 통해 공유해요.
3. BUFFER_SIZE를 10으로 설정했고, 인덳는 아래와 같이 작동해요.
   - 데이터를 새로 저장한다. => `end_addr = (end_addr + 1) % BUFFER_SIZE`
        - 만약 새로 저장하려는 end_addr값이 start_adder인 경우 => `start_addr = (start_addr + 1) % BUFFER_SIZE`


## 🕹️실행
1. 실행 후, 저장할 값을 입력합니다.
2. 복구하고 싶을 때, Ctrl+C를 통해 시그널을 호출합니다.
3. 아래 사진 처럼 그동안 저장한 데이터를 목록으로 받아볼 수 있어요.
4. 돌아가고 싶은 지점을 선택해주세요.
   - ex) 입력: 2 = arr[1]부터 저장

<img width="421" alt="image" src="https://github.com/user-attachments/assets/9ead5d8c-bcd6-4b00-87fc-1d10a129317b">

## 🔥주의 사항
1. 문자열로 입력받기 때문에, 기존 테스트 코드처럼 문자를 입력해도 종료되지 않아요.
2. 데이터 배열을 모두 순회한 경우에는 다음과 같이 작동해요.

입력 값
```
1
2
3
4
5
6
7
8
9
10
11
12
^c
2
```

<img width="425" alt="image" src="https://github.com/user-attachments/assets/fc9e772a-af3e-4543-9370-19fda7ec3024">
