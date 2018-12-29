/*Aspek-aspek teknis yang digunakan :*/
//1. Pemrograman modular. Program yang dibuat menggunakan fungsi-fungsi yang dibuat diluar main program. Fungsi-fungsi tersebut dapat digunakan pada program lain yang membutuhkan fungsi spesifik yang sama.
//2. Array. Array pada program ini digunakan untuk menyimpan beberapa form pada variable yang sama. digunakan juga untuk menyimpan sementara data untuk disortir.
//3. Sorting. Sorting digunakan pada fungsi sortingData, sebelum program menampilan hasil responden form.
//4. Pointer. Digunakan untuk membuat array dan string data type. Digunakan juga untuk membuat struktur node pada linked list.
//5. Linked list. Digunakan untuk memnyimpan data-data form, pertanyaan, serta datanya.

/*Modul-modul standard c yang dibutuhkan program untuk menjalankan fungsinya*/
#include <stdlib.h>  //alokasi memory
#include <stdio.h>   //input output program (printf, scanf)
#include <conio.h>   //getch
#include <string.h>  //memproses tipe data string
#include <windows.h> //mendapatkan variable lebar command line

//Berikut struktur data yang digunakan program untuk menyimpan form serta data-datanya. Data disimpan sebagai linked list. Form Information disimpan sebagai head, dan pertanyaan pertanyaan disimpan sebagai nodenya.

//node form hanya digunakan sebagai head.
typedef struct form{
    char *name;         //Untuk menyimpan nama form
    char *desc;         //Untuk menyimpan deskripsi form tersebut
    struct qp *qpNxt;   //sebagai penunjuk ke node pertanyaan.
    struct qc *qcNxt;
} form;

//Node untuk menyimpan pertanyaan-pertanyaan yang ada dalam form. Terdapat 2 jenis pertanyaan yang dapat memiliki spesifikasinya masing-masing sehingga dibuat sebagai 2 buah jenis node yang terpisah.
typedef struct qp{
    int order;          //Untuk menentukan urutan pertanyaan
    char *question;     //Untuk menyimpan pertanyaan
    int scale;          //Menyimpan skala pertanyaan
    int *data;          //Menyimpan data responden
    struct qp *nxt;     //Menunjuk ke pertanyaan selanjutnya yang sejenis
} qp;

typedef struct qc{
    int order;
    char *question;
    char **choices;     //Menyimpan pilihan-pilihan yang akan ditawarkan oleh pertanyaan
    int noChoices;      //Jumlah pilihan yang ada
    int *data;
    struct qc *nxt;
} qc;

//pointer-pointer yang menunjuk kepada node. digunakan untuk memudahkan alokasi memori dan pembuatan linked list.
typedef qc *qcPtr;
typedef qp *qpPtr;
typedef form *formPtr;

//variable-variable global yang digunakan program
int screenWidth; //Menyimpan lebar command line
char temp[255];  //untuk menyimpan input sementara, digunakan untuk menentukan panjang string sebelum dialokasi memorinya, dan juga untuk menentukan tipe data sebelum disimpan pada variable tertentu untuk menghindari error
FILE *dat;       //variable file eksternal yang digunakan untuk menyimpan state program

//Berikut fungsi-fungsi utama yang digunakan untuk mengolah data dan menjalankan fungsi utama program. Memiliki banyak dependensi pada fungsi-fungsi Penunjang. 
void createNewForm(formPtr *ptr, int *noForm);  //Membuat form baru
void formPreview(formPtr ptr);                  //preview form yang sedang dibuat
void addNewSQ(formPtr *ptr, int *order);        //membuat pertanyaan skalar baru
void addNewMC(formPtr *ptr, int *order);        //membuat pertanyaan pilihan ganda baru
void fillForm(formPtr ptr);                     //mengisi form sebagai responden
void showResult(formPtr ptr);                   //menampilkan hasil form yang telah diisi oleh responden
void sortData(int array[][2], int n);           //mengurutkan data sebelum ditampilkan
void updateData(formPtr *ptr, int noForm);      //mengupdate data pada file eksternal. digunakan setiap terjadi perubahan pada data.
int loadData(formPtr **ptr, int *n);           //load data eksternal kememory. digunakan pada awal menjalankan program.

//berikut fungsi-fungsi penunjang yang digunakan sebagai user interface dan input management.
void init(formPtr **ptr, int *n);          //inisiasi program sebelum program dijalankan. Mengambil variable lebar command line dan loadData
void printTitle(char *title);              //untuk mencetak template main title
void printSubTitle(char *subTitle);        //untuk mencetak template semi title
void printLine();                          //untuk mencetak garis sepanjang lebar command line
int inputNumber(char *input);              //untuk melakukan input angka. akan mengembalikan -1 jika input bukan angka
void inputString(char *input);             //untuk melakukan input string. digunakan agar dapat memproses input spasi
void printBar(int total, int data);        //digunakan untuk mencetak progress bar untuk membandingkan jumlah responden secara visual

//main program
void main(){
    int noForm = 0;         //variable yang menyimpan jumlah form yang sudah dibuat. diinisiasi dengan nilai 0
    formPtr *forms;         //array yang digunakan untuk menyimpan form
    init(&forms, &noForm);  //inisiasi program. load form yang telah dibuat sebelumnya
    int menu;               //variable input sebagai navigasi menu
    int i;                  //variable iterasi
    
    //Main loop. Program akan terus menjalankan bagian ini sampai user memilih untuk keluar.
    do{
        system("CLS");                 //reset screen state
        printTitle("||\t\t\t\t\t    SELAMAT DATANG DI PROGRAM FORM\t\t\t\t\t      ||\n"
		"||\t\t\t\t\t   Oleh: DEDE KOSASIH - 1706985930\t\t\t\t\t      ||\n"
		"||\t\t\t\t\t     HILMAN MAULANA - 1706985975\t\t\t\t\t      ||");
        printf(" 1. Create New Form\n 2. Fill Form\n 3. Form Result\n 0. Exit\n");  //Mencetak main menu
        printLine();
        printf(">");
        inputString(temp);         
        menu = inputNumber(temp);    //input menu navigasi

        //pilihan menu 1. membuat form baru
        if (menu==1){
            //alokasi memori form baru pada array yang menimpan head form
            if (noForm==0){
                forms = malloc(sizeof(formPtr)*1);
                forms[0]=malloc(sizeof(form));
            }
            else{
                forms = realloc(forms, sizeof(formPtr)*(noForm + 1));
                forms[noForm]=malloc(sizeof(form));
            }
            //menjalankan fungsi buat form baru kemudian file eksternal diupdate.
            createNewForm(&forms[noForm], &noForm);
            updateData(forms, noForm);
        }

        //pilihan menu 2. mengisi form
        else if (menu==2){
            //menu 2 loop. program akan terus mengulang bagian ini sampai user memilih untuk kembali ke menu utama
            do{
                system("CLS");
                printSubTitle("Fill Form");
                printf("\nChose form to fill : \n\n");
                //program akan menampilkan seluruh nama form yang telah dibuat
                if(noForm>0){
                    for (i=0;i<noForm;i++){
                        printf(" %d. %s\n", i+1, forms[i]->name);
                    }
                }
                printf(" 0. Back to main menu\n");
                printLine();
                printf(">");
                inputString(temp);
                menu = inputNumber(temp);
                
                //jika input valid user akan di nanvigasi ke fungsi isi form sesuai form yang yang dipilih
                if(menu>0 && menu<=noForm){
                    system("CLS");
                    fillForm(forms[menu-1]);
                    updateData(forms, noForm);
                    printLine();
                    printf("Thank you for your response, press anykey to continue...");
                    getch();
                }
            }while(menu!=0);
            menu=-1;
        }

        //pilihan menu 3. menampilkan hasil form
        else if (menu==3){
            do{
                system("CLS");
                printSubTitle("Form Result");
                printf("\nChose form : \n\n");
                //menampilkan seluruh nama form yang tersedia
                if(noForm>0){
                    for (i=0;i<noForm;i++){
                        printf(" %d. %s\n", i+1, forms[i]->name);
                    }
                }
                printf(" 0. Back to main menu\n");
                printLine();
                printf(">");
                inputString(temp);
                menu = inputNumber(temp);

                //jika input valid, program akan menampilkan hasil form yang dipilih user
                if(menu>0 && menu<=noForm){
                    system("CLS");
                    showResult(forms[menu-1]);
                    printLine();
                    printf("Press anykey to go back...");
                    getch();
                }
            }while(menu!=0);
            menu=-1;
        }
        else{
            //input yang tidak valid akan diabaikan oleh program.
        }
    }while(menu!=0);
}

//realisasi fungsi untuk membuat form baru
void createNewForm(formPtr *ptr, int *noForm){
    int menu;              //variable navigasi menu
    int order = 0;         //variable untuk menentukan urutan pertanyaan
    system("CLS");
    printSubTitle("Create New Form");
    
    //user diminta untuk input nama form. kemudian jumlah karakter akan dihitung dan memori akan dialokasi sesuai jumlah karakter yang dibutuhkan untuk menyimpan nama, baru kemudian akan disimpan oleh program
    printf("Form Name : ");
    inputString(temp);
    (*ptr)->name = malloc(sizeof(char)*strlen(temp));
    strcpy((*ptr)->name, temp);

    //user diminta untuk input deskripsi form. kemudian jumlah karakter akan dihitung dan memori akan dialokasi sesuai jumlah karakter yang dibutuhkan untuk menyimpan deskripsi, baru kemudian akan disimpan oleh program
    printf("Form Description : ");
    inputString(temp);
    (*ptr)->desc = malloc(sizeof(char)*strlen(temp));
    strcpy((*ptr)->desc, temp);

    //inisiasi untuk node pertanyaan ke null.
    (*ptr)->qpNxt=NULL;
    (*ptr)->qcNxt=NULL;

    //loop pada fungsi buat form baru. program akan terus mengulang bagian ini sampai user memilih untuk menyimpan atau membuang form yang telah dibuat.
    do{
        system("CLS");
        printSubTitle("Create New Form");

        //mencetak nama dan deskripsi, serta menu pada pembuatan form baru
        printf("Form Name : %s\n", (*ptr)->name);
        printf("Form Description : %s\n", (*ptr)->desc);
        printLine();
        printf(" 1. Add Multiple Choices Question\n 2. Add Scalar Question\n 3. Form Preview\n 9. Finish and Save Form\n 0. Discard Form\n");
        printLine();
        printf(">");
        inputString(temp);
        menu = inputNumber(temp);

        //pilihan menu 1. buat pertanyaan pilihan ganda baru
        if (menu==1){
            addNewMC(ptr, &order);
        }

        //pilihan menu 2. buat pertanyaan skalar baru
        else if (menu==2){
            addNewSQ(ptr, &order);
        }

        //pilihan menu 3. preview form yang sedang dibuat
        else if(menu==3){
            system("CLS");
            formPreview(*ptr);
            printLine();
            printf("Press anykey to go back...");
            getch();
        }

        //pilihan menu 0. buang form yang sedang dibuat.
        else if (menu==0){
            printf("Press 0 again to discard...");
            menu=getch()-48;
        }

        //pilihan menu 9. selesai dan simpan form yang sedang dibuat
        else if (menu==9){
            *noForm = *noForm + 1;
        }

        //abaikan input yang tidak valid.
    }while(menu!=0&&menu!=9);
}

//realisasi fungsi untuk membuat pertanyaan skalar
void addNewSQ(formPtr *ptr, int *order){
    qpPtr cur, new;  //variable pointer sebagai penunjuk ke current pointer dan pertanyaan baru yang akan dibuat
    int i;           //variable iterasi
    cur=NULL;        //inisiasi current pointer dengan null

    //alokasi memori untuk pertanyaan baru
    new = malloc(sizeof(qp));
    new->order=*order;   //menyimpan  urutan pertanyaan
    new->nxt=NULL;

    system("CLS");
    printSubTitle("Add New Scalar Question");

    //user diminta untuk input pertanyaan form. kemudian jumlah karakter akan dihitung dan memori akan dialokasi sesuai jumlah karakter yang dibutuhkan untuk menyimpan pertanyaan, baru kemudian akan disimpan oleh program
    printf("Question : ");
    inputString(temp);
    new->question = malloc(sizeof(char)*strlen(temp));
    strcpy(new->question, temp);

    //input skala pertanyaan. progran akan mengulang proses ini sampai input yang dimasukkan user valid.
    do{
        printf("Scale (min 2): ");
        inputString(temp);
        new->scale = inputNumber(temp);
    }while(new->scale < 2);

    //alokasi memori pada array data sesuai dengan skala yang dibutuhkan, dan diinisiasi dengan nilai 0.
    new->data = malloc(sizeof(int)*new->scale);
    for (i=0;i<new->scale;i++){
        new->data[i] = 0;
    }

    //menaruh node pertanyaan baru di paling belakang linked list pertanyaan dengan jenis ini
    if ((*ptr)->qpNxt==NULL){
        (*ptr)->qpNxt = new;
    }
    else{
        cur = (*ptr)->qpNxt;
        while(cur->nxt!=NULL){
            cur=cur->nxt;
        }
        cur->nxt=new;
    }

    *order = *order + 1; //urutan pertanyaan diincrement
    printf("Question Successfully Added...");
    getch();
}

//realisasi fungsi untuk membuat pertanyaan pilihan ganda
void addNewMC(formPtr *ptr, int *order){
    qcPtr cur, new;   //pointer yang menunjuk ke current pointer dan pertanyaan baru
    int i;
    cur=NULL;

    //alokasi memori untuk pertanyaan baru
    new = malloc(sizeof(qc));
    new->order=*order;
    new->nxt=NULL;

    system("CLS");
    printSubTitle("Add New Multiple Choice Question");

    //user diminta untuk input pertanyaan form. kemudian jumlah karakter akan dihitung dan memori akan dialokasi sesuai jumlah karakter yang dibutuhkan untuk menyimpan pertanyaan, baru kemudian akan disimpan oleh program
    printf("Question : ");
    inputString(temp);
    new->question = malloc(sizeof(char)*strlen(temp));
    strcpy(new->question, temp);

    //user memasukkan jumlah pilihan yang akan disediakan, program akan mengulang proses ini sampai input valid.
    do{
        printf("Number of Choices (min 2): ");
        inputString(temp);
        new->noChoices = inputNumber(temp);
    }while(new->noChoices < 2);

    //alokasi memori untuk menyimpan data pilihan serta pilihannya. user akan diminta memasukkan setiap pilihan dan program akan menginisiasi data pilihan dengan nilai 0.
    new->data = malloc(sizeof(int)*new->noChoices);
    new->choices = malloc(sizeof(char*)*new->noChoices);
    for (i=0;i<new->noChoices;i++){
        printf("Choice %d : ", i+1);
        inputString(temp);
        new->choices[i] = malloc(sizeof(char)*strlen(temp));
        strcpy(new->choices[i], temp);
        new->data[i] = 0;
    }

    //menaruh pertanyaan pada tempatnya
    if ((*ptr)->qcNxt==NULL){
        (*ptr)->qcNxt = new;
    }
    else{
        cur = (*ptr)->qcNxt;
        while(cur->nxt!=NULL){
            cur=cur->nxt;
        }
        cur->nxt=new;
    }

    *order = *order + 1; //increment variable urutan
    printf("Question Successfully Added...");
    getch();
}

//realisasi fungsi mengisi form
void fillForm(formPtr ptr){

    //program mencetak nama serta deskripsi form
    printSubTitle(ptr->name);
    printf("Form Description : %s\n", ptr->desc);
    printLine();
    printf("\n");

    //variable-variable yang digunakan dalam fungsi
    qpPtr curQp = ptr->qpNxt;  //current pointer untuk setiap jenis pertanyaan
    qcPtr curQc = ptr->qcNxt;
    int order = 0;             //variable urutan pertanyaan yang akan diiterasi. dimulai dari 0
    int i;                     //variabel iterasi
    int resp;                  //variable navigasi fungsi serta respon yang akan dimasukkan ke dalam data

    //program akan mencetak setiap pertanyaan secara berurutan dengan cara membandingkan variable urutan fungsi dengan nilai urutan yang terdapat pada node pertanyaan.
    while (curQp!=NULL || curQc!=NULL){
        if (curQp!=NULL && curQp->order==order){

            //mencetak pertanyaan serta keterangan skalanya
            printf(" %d. %s (1-%d)\n", order+1, curQp->question, curQp->scale);

            //respon user. program akan terus mengulan bagian ini sampai input valid
            do{
                printf(">");
                inputString(temp);
                resp=inputNumber(temp);
                if(resp<1||resp>curQp->scale){
                    printf("Input is not valid, try again...\n");
                }
                else{
                    curQp->data[resp - 1] = curQp->data[resp - 1] +1;
                    printf("\n");
                    break;
                }
            }while(1);

            //geser current pointer untuk jenis pertanyaan ini
            curQp = curQp->nxt;
        }
        else if (curQc!=NULL && curQc->order==order){

            //mencetak pertanyaan serta pilihan yang disediakan
            printf(" %d. %s\n", order+1, curQc->question);
            for (i=0;i<curQc->noChoices;i++){
                printf("\t%c. %s\n", i+97, curQc->choices[i]);
            }

            //respon user. program akan mengulan bagian ini sampai input valid
            do{
                printf(">");
                inputString(temp);
                resp=temp[0] - 96;
                if(resp<1||resp>curQc->noChoices){
                    printf("Input is not valid, try again...\n");
                }
                else{
                    curQc->data[resp - 1] = curQc->data[resp - 1] +1;
                    printf("\n");
                    break;
                }
            }while(1);

            //geser current pointer jenis pertanyaan ini
            curQc = curQc->nxt;
        }

        order++; //increment variable urutaan untuk melanjutkan ke pertanyaan berikutnya
    }
}

//realisasi fungsi yang menampilkan hasil form
void showResult(formPtr ptr){

    //menampilkan nama serta deskripsi form
    strcpy(temp, "Result ");
    strcat(temp, ptr->name);
    printSubTitle(temp);
    printf("Form Description : %s\n", ptr->desc);
    printLine();
    printf("\n");

    //inisiasi variable yang digunakan fungsi
    qpPtr curQp = ptr->qpNxt;   //current pointer untuk setiap jenis pertannyaan
    qcPtr curQc = ptr->qcNxt;
    int order = 0;              //variable untuk mengurutkan pertanyaan
    int i, tot=0;               //variable iterasi dan jumlah responden

    //program akan menampilan hasil untuk setiap pertanyaan secara berurutan. untuk setiap pertanyaan, dibuat array sementara untuk menyimpan data pertanyaan dan pengurutan
    while (curQp!=NULL || curQc!=NULL){
        if (curQp!=NULL && curQp->order==order){
            int total=0;
            printf(" %d. %s (1-%d)\n", order+1, curQp->question, curQp->scale);
            int tempArr[curQp->scale][2];
            for (i=0;i<curQp->scale;i++){
                tempArr[i][0]= i+1; tempArr[i][1]= curQp->data[i];
                total=total+curQp->data[i];
            }
            sortData(tempArr, curQp->scale);
            for (i=0;i<curQp->scale;i++){
                printBar(total, tempArr[i][1]);
                printf(" %d : %d\n", tempArr[i][0], tempArr[i][1]);
            }
            printf("\n");
            tot=total;
            curQp = curQp->nxt;
        }
        else if (curQc!=NULL && curQc->order==order){
            int total=0;
            printf(" %d. %s\n", order+1, curQc->question);
            int tempArr[curQc->noChoices][2];
            for (i=0;i<curQc->noChoices;i++){
                tempArr[i][0]= i; tempArr[i][1]= curQc->data[i];
                total=total+curQc->data[i];
            }
            sortData(tempArr, curQc->noChoices);
            for (i=0;i<curQc->noChoices;i++){
                printBar(total, tempArr[i][1]);
                printf(" %s : %d\n", curQc->choices[tempArr[i][0]], tempArr[i][1]);
            }
            printf("\n");
            tot=total;
            curQc = curQc->nxt;
        }
        order++;
    }
    printLine();
    printf("Total Responden : %d\n", tot);  //menampilkan jumlah responden
}

//realisai fungsi yang menampilkan preview form yang sedang dibuat
void formPreview(formPtr ptr){

    //mencetak nama serta deskripsi form
    strcpy(temp, "Preview ");
    strcat(temp, ptr->name);
    printSubTitle(temp);
    printf("Form Description : %s\n", ptr->desc);
    printLine();
    printf("\n");

    //variable yang digunakan fungsi
    qpPtr curQp = ptr->qpNxt;
    qcPtr curQc = ptr->qcNxt;
    int order = 0;
    int i;

    //menampilkan pertanyaan yang telah dibuat sesuai dengan urutannya
    while (curQp!=NULL || curQc!=NULL){
        if (curQp!=NULL && curQp->order==order){
            printf(" %d. %s (1-%d)\n\n", order+1, curQp->question, curQp->scale);
            curQp = curQp->nxt;
        }
        else if (curQc!=NULL && curQc->order==order){
            printf(" %d. %s\n", order+1, curQc->question);
            for (i=0;i<curQc->noChoices;i++){
                printf("\t%c. %s\n", i+97, curQc->choices[i]);
            }
            printf("\n");
            curQc = curQc->nxt;
        }
        order++;
    }
}

//realisasi fungsi untuk mengurutkan data pertanyaan sebelum ditampilkan
void sortData(int array[][2], int n){
    int min, i, k, j[2];

    //menggunakan sorting bubble
    for (i=0;i<n-1;i++){
        min = i;
        for (k=i+1;k<n;k++){
            if(array[min][1]<array[k][1]){
                min = k;
            }
        }
        if (min!=i){
            j[0]=array[min][0] ;j[1]= array[min][1];
            array[min][0] =array[i][0];array[min][1]= array[i][1];
            array[i][0] =j[0]; array[i][1]= j[1];
        }
    }
}

//realisasi fungsi untuk load data eksternal ke dalam memori
int loadData(formPtr **ptr, int *n){
    //program menjalakan kemmbali form-form yang telah dibuat sebelumnya dengan cara melakukan proses pembuatan form tersebut berdasarkan data yang ada di file eksternal.

    //variable yang digunakan dalam fungsi
    int i,j, noForm=0;                //variable iterasi dan jumlah form yang telah dibuat
    qpPtr newQp = NULL;               //pointer untuk membuat pertanyaan
    qcPtr newQc = NULL;

    //membuka file eksternal dengan mode membaca. jika file tidak tersedia, fungsi akan berakhir dan program akan berjalan tanpa form apapun dalam memori.
    dat = fopen("data.dat", "r+");
    if (!dat){
        return 0;
    }

    //membaca line pertama sebagai jumlah form yang telah dibuat. fungsi akan berakhir jika tidak ada form yang tersedia di file eksternal
    fgets(temp, 255, (FILE*)dat);
    temp[strlen(temp)-1]='\0';
    noForm=inputNumber(temp);
    *n=noForm;
    if (noForm==0){
        return 0;
    }

    //alokasi memori sesuai form yang ada
    (*ptr)=malloc(sizeof(formPtr)*noForm);
    for (i=0;i<noForm;i++){
        (*ptr)[i]=malloc(sizeof(form));
    }

    //program membaca setiap line yang ada di file ekseternal secara berurutan, yang urutannya telah dibuat sedemikian rupa sehingga dapat dilakukan pembuatan form didalam program, beserta stiap pertanyaan dan data yang dibawanya.

    //loop untuk membaca setiap form yang tersedia di file eksternal
    fgets(temp, 255, (FILE*)dat);
    for (i=0;i<noForm;i++){

        //inisiasi current pointer untuk setiap jenis pertanyaan
        qpPtr curQp = NULL;
        qcPtr curQc = NULL;

        //line yang berisi nama form akan dimasukkan kedalam nama form pada program
        fgets(temp, 255, (FILE*)dat);
        temp[strlen(temp)-1]='\0';
        (*ptr)[i]->name=malloc(sizeof(char)*strlen(temp));
        strcpy((*ptr)[i]->name, temp);

        //line yang berisi deskripsi form akan dimasukkan kedalam deskripsi form pada program
        fgets(temp, 255, (FILE*)dat);
        temp[strlen(temp)-1]='\0';
        (*ptr)[i]->desc=malloc(sizeof(char)*strlen(temp));
        strcpy((*ptr)[i]->desc, temp);

        //pada form yang sedang dibuat, pointer yang menunjuk ke node pertanyaan diinisiasi dengan null
        (*ptr)[i]->qcNxt=NULL;
        (*ptr)[i]->qpNxt=NULL;

        fgets(temp, 255, (FILE*)dat);
        temp[strlen(temp)-1]='\0';

        //loop unutk membaca setiap pertanyaan yang ada
        do{

            //jika line mengindikasikan bahwa pertanyaan adalah pertanyaan skalar, maka akan dibuat pertanyan dengan jenis tersebut
            if (strcmp("qp", temp)==0){
                newQp=malloc(sizeof(qp));
                newQp->nxt=NULL;
                fgets(temp, 255, (FILE*)dat);
                temp[strlen(temp)-1]='\0';
                newQp->order=inputNumber(temp);
                fgets(temp, 255, (FILE*)dat);
                temp[strlen(temp)-1]='\0';
                newQp->question=malloc(sizeof(char)*strlen(temp));
                strcpy(newQp->question, temp);
                fgets(temp, 255, (FILE*)dat);
                temp[strlen(temp)-1]='\0';
                newQp->scale=inputNumber(temp);
                newQp->data=malloc(sizeof(int)*newQp->scale);
                for(j=0;j<newQp->scale;j++){
                    fgets(temp, 255, (FILE*)dat);
                    temp[strlen(temp)-1]='\0';
                    newQp->data[j]=inputNumber(temp);
                }
                if (curQp==NULL){
                    (*ptr)[i]->qpNxt=newQp;
                    curQp=(*ptr)[i]->qpNxt;
                }
                else{
                    curQp->nxt=newQp;
                    curQp=curQp->nxt;
                }
            }

            //jika line mengindikasikan bahwa pertanyaan adalah pertanyaan pilihan ganda, maka akan dibuat pertanyan dengan jenis tersebut
            else if (strcmp("qc", temp)==0){
                newQc=malloc(sizeof(qc));
                newQc->nxt=NULL;
                fgets(temp, 255, (FILE*)dat);
                temp[strlen(temp)-1]='\0';
                newQc->order=inputNumber(temp);
                fgets(temp, 255, (FILE*)dat);
                temp[strlen(temp)-1]='\0';
                newQc->question=malloc(sizeof(char)*strlen(temp));
                strcpy(newQc->question, temp);
                fgets(temp, 255, (FILE*)dat);
                temp[strlen(temp)-1]='\0';
                newQc->noChoices=inputNumber(temp);
                newQc->data=malloc(sizeof(int)*newQc->noChoices);
                newQc->choices=malloc(sizeof(char*)*newQc->noChoices);
                for(j=0;j<newQc->noChoices;j++){
                    fgets(temp, 255, (FILE*)dat);
                    temp[strlen(temp)-1]='\0';
                    newQc->choices[j]=malloc(sizeof(char)*strlen(temp));
                    strcpy(newQc->choices[j], temp);
                }
                for(j=0;j<newQc->noChoices;j++){
                    fgets(temp, 255, (FILE*)dat);
                    temp[strlen(temp)-1]='\0';
                    newQc->data[j]=inputNumber(temp);
                }
                if (curQc==NULL){
                    (*ptr)[i]->qcNxt=newQc;
                    curQc=(*ptr)[i]->qcNxt;
                }
                else{
                    curQc->nxt=newQc;
                    curQc=curQc->nxt;
                }
            }
            fgets(temp, 255, (FILE*)dat);
            temp[strlen(temp)-1]='\0';
        }while(strcmp("end", temp)!=0&&strcmp("form", temp)!=0);
        //fungsi akan berakhir jika seluruh form yang ada pada file eksternal telah dibuat
    }
    fclose(dat); //tutup file
    return 1;
}

//realisasi fungsi untuk update data pada file eksternal
void updateData(formPtr *ptr, int noForm){
    //fungsi ini dibuat dengan cara mencetak seluruh data yang dibutuhkan program untuk membuat seluruh form yang ada dari 0.

    //membuka file eksternal dengan mode write. line pertama pada file eksternal ini digunakan untuk menyimpan jumlah form yang dibuat.
    dat = fopen("data.dat", "w+");
    sprintf(temp, "%d", noForm);
    fputs(temp, dat);
    fputs("\n", dat);

    int i, j;  //variable iterasi

    //untuk setiap form, akan dicetak seluruh informasinya secara berurutan.
    for (j=0;j<noForm;j++){
        fputs("form", dat);
        fputs("\n", dat);
        fputs(ptr[j]->name, dat);
        fputs("\n", dat);
        fputs(ptr[j]->desc, dat);
        fputs("\n", dat);

        qpPtr curQp = ptr[j]->qpNxt;
        qcPtr curQc = ptr[j]->qcNxt;
        int order = 0;

        //seluruh informasi pertanyaan juga dicetak secara berurutan.
        while (curQp!=NULL || curQc!=NULL){
            if (curQp!=NULL && curQp->order==order){
                fputs("qp", dat);
                fputs("\n", dat);
                sprintf(temp, "%d", order);
                fputs(temp, dat);
                fputs("\n", dat);
                fputs(curQp->question, dat);
                fputs("\n", dat);
                sprintf(temp, "%d", curQp->scale);
                fputs(temp, dat);
                fputs("\n", dat);
                for (i=0;i<curQp->scale;i++){
                    sprintf(temp, "%d", curQp->data[i]);
                    fputs(temp, dat);
                    fputs("\n", dat);
                }
                curQp = curQp->nxt;
            }
            else if (curQc!=NULL && curQc->order==order){
                fputs("qc", dat);
                fputs("\n", dat);
                sprintf(temp, "%d", order);
                fputs(temp, dat);
                fputs("\n", dat);
                fputs(curQc->question, dat);
                fputs("\n", dat);
                sprintf(temp, "%d", curQc->noChoices);
                fputs(temp, dat);
                fputs("\n", dat);
                for (i=0;i<curQc->noChoices;i++){
                    fputs(curQc->choices[i], dat);
                    fputs("\n", dat);
                }
                for (i=0;i<curQc->noChoices;i++){
                    sprintf(temp, "%d", curQc->data[i]);
                    fputs(temp, dat);
                    fputs("\n", dat);
                }
                curQc = curQc->nxt;
            }
            order++;
        }
    }
    //fungsi akan berakhir ketika seluruh data telah ditulis ke dalam file eksternal, dan diakhiri dengan "end\n" sebagai penanda
    fputs("end\n", dat);
    fclose(dat);  //tutup file 
}

//realisai fungsi inisiasi
void init(formPtr **ptr, int *n){

    //mendapatkan lebar command line untuk dipakai pada fungsi lain yang berhubungan dengan ui
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int ret;
    ret = GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ),&csbi);
    if(ret)
    {
        screenWidth = csbi.dwSize.X;
    }
    else{
        screenWidth = 80;
    }

    //load program state
    loadData(ptr, n);
}

//realisasi fungsi untuk mencetak judul utama
void printTitle(char *title){
    //fungsi ini digunakan untuk mencetak judul utama dengan spesifikasi center dan diapit "=" sepanjang lebar command line

    int i;
    int numOfSpace=(screenWidth-strlen(title))/2;
    for (i=0;i<screenWidth;i++){
        printf("=");
    }
    for (i=0;i<numOfSpace;i++){
        printf(" ");
    }
    printf("%s\n", title);
    for (i=0;i<screenWidth;i++){
        printf("=");
    }
    printf("\n");
}

//realisasi fungsi untuk mencetak garis sepanjang lebar command line
void printLine(){
    //fungsi ini akan mencetak "_" sepanjang lebar command line

    int i;
    for (i=0; i<screenWidth;i++){
        printf("_");
    }
}

//realisasi fungsi untuk mencetak semi judul
void printSubTitle(char *subTitle){
    //fungsi ini digunakan untuk mencetak semi judul dengan spesifikasi semi judul akan diapit oleh "-" sepanjang semijudul

    int i;
    for (i = 0; i<strlen(subTitle);i++){
        printf("-");
    }

    printf("\n%s\n", subTitle);

    for (i = 0; i<strlen(subTitle);i++){
        printf("-");
    }

    printf("\n");
}

//realisasi fungsi untuk input angka
int inputNumber(char *input){
    //fungsi ini dibuat untuk mendeteksi apakan input dari user adalah angka atau bukan, sehingga menghindari error yang terjadi akibat kesalahan tipe data.

    int tempNumber=0;
    int i;

    //input dibaca sebagai string, dan akan dicek setiap karakternya, jika ada karakter yang bukan angka, maka input bukan angka adn akan mengembalikan nilai -1.
    for (i=0;i<strlen(input);i++){
        if (input[i]<48||input[i]>57){
            return -1;
        }
        else{
            tempNumber=tempNumber*10+input[i]-48;
        }
    }
    //jika input angka, akan dikembalikan nilai angka tersebut
    return tempNumber;
}

//realisasi fugnsi untuk input string
void inputString(char *input){
    //fungsi ini dibuat untuk membaca tipe data string yang tidak menggunakan spasi sebagai input separator. digunakan pada setiap proses input pada program ini dengan tujuan membaca seluruh string termasuk spasi.

    //proses input akan terus diulang sampai user memasukkan sedikitnya satu karakter.
    do{
        fgets(input, 256, stdin);
        input[strlen(input)-1]='\0';
    }while(strlen(input)<1);
}

//realisasi fungsi untuk mencetak bar
void printBar(int total, int data){
    //fungsi ini dibuat untuk membandingkan jumlah data secara visual.

    int i;
    if (total==0){
        total=1;
    }
    int maxBar=40;

    //mencetak karakter "*" sejumlah yang sesuai dengan nilai data relatif terhadap jumlah data
    printf("[");
    for (i=0;i<data*maxBar/total;i++){
        printf("*");
    }
    for (i=0;i<maxBar-data*maxBar/total;i++){
        printf(" ");
    }
    printf("]");
}
