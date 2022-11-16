#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../include/string_parser.h"

char sp[42] = "----------------------------------------\n\0";

/**
 * Tests get_path_until_tar
 * On affiche que les cas où il y
 * a une erreur
 */
void tests_get_path_until_tar()
{
    printf("%sDébut des tests get_path_until_tar\n\n", sp);

    char *full = get_path_until_tar("abc/def/toto.tar/ghi/jkl");
    char *full1 = get_path_until_tar("/abc/toto.tar/def");
    char *endsWithSlash = get_path_until_tar("abc/def/toto.tar/ghi/jkl/");
    char *full_many = get_path_until_tar("abc///def///toto.tar///ghi///jkl");
    char *empty = get_path_until_tar("");
    char *space = get_path_until_tar(" ");
    char *noTar = get_path_until_tar("abc/def/ghi/jkl");
    char *startsWithTar = get_path_until_tar("toto.tar/ghi/jkl");
    char *endsWithTar = get_path_until_tar("abc/def/toto.tar");
    char *endsWithTarSlash = get_path_until_tar("abc/def/toto.tar/");
    char *onlyTar = get_path_until_tar("toto.tar");
    char *slashOnlyTar = get_path_until_tar("/toto.tar");
    char *onlyTarSlash = get_path_until_tar("toto.tar/");

    // Test outuput
    printf("Test échoués :\n");
    if (strcmp(full, "abc/def/toto.tar") != 0)
        printf("1-full : %s\n", full);
    if (strcmp(full1, "/abc/toto.tar") != 0)
        printf("2-full1 : %s\n", full1);
    if (strcmp(endsWithSlash, "abc/def/toto.tar") != 0)
        printf("3-endsWithSlash : %s\n", endsWithSlash);
    if (strcmp(full_many, "abc///def///toto.tar") != 0)
        printf("4-full_many : %s\n", full_many);
    if (empty != NULL)
        printf("5-empty : %s\n", empty);
    if (space != NULL)
        printf("6-space : %s\n", space);
    if (noTar != NULL)
        printf("7-noTar : %s\n", noTar);
    if (strcmp(startsWithTar, "toto.tar") != 0)
        printf("8-startsWithTar : %s\n", startsWithTar);
    if (strcmp(endsWithTar, "abc/def/toto.tar") != 0)
        printf("9-endsWithTar : %s\n", endsWithTar);
    if (strcmp(endsWithTarSlash, "abc/def/toto.tar") != 0)
        printf("10-endsWithTarSlash : %s\n", endsWithTarSlash);
    if (strcmp(onlyTar, "toto.tar") != 0)
        printf("11-onlyTar : %s\n", onlyTar);
    if (strcmp(slashOnlyTar, "/toto.tar") != 0)
        printf("12-slashOnlyTar : %s\n", slashOnlyTar);
    if (strcmp(onlyTarSlash, "toto.tar") != 0)
        printf("13-onlyTarSlash : %s\n", onlyTarSlash);

    // Freeing memory
    free(full);
    free(full1);
    free(endsWithSlash);
    free(full_many);
    free(startsWithTar);
    free(endsWithTar);
    free(endsWithTarSlash);
    free(onlyTar);
    free(slashOnlyTar);
    free(onlyTarSlash);

    printf("\nFin des tests get_path_until_tar\n%s\n", sp);
}

/**
 * Tests get_path_until_tar
 */
void tests_str_to_arr() {
    printf("%sDébut des tests str_to_arr\n\n", sp);

    char **normal = str_to_arr("cmd1 | cmd2 | cmd3", '|');
    int len_1 = get_path_length("cmd1 | cmd2 | cmd3", '|');
    char **consecutive_sep = str_to_arr("cmd1 ||| cmd2 || cmd3", '|');
    int len_2 = get_path_length("cmd1 ||| cmd2 || cmd3", '|');
    char **nothing_before_sep = str_to_arr("| some command", '|');
    int len_3 = get_path_length("| some command", '|');
    char **nothing_after_sep = str_to_arr("some command |", '|');
    int len_4 = get_path_length("some command |", '|');
    char **separators_only = str_to_arr("||", '|');
    int len_5 = get_path_length("||", '|');
    char **separator_not_found = str_to_arr("blabla blabla", '|');
    int len_6 = get_path_length("blabla blabla", '|');
    char **empty = str_to_arr("", '|');
    int len_7 = get_path_length("", '|');


    // Display output with '|' after each char*
    printf("Len normal : %d\n", len_1);
    for (int i = 0; i < len_1; i++)
        printf("%s|", normal[i]);
    printf("\nLen consecutive_sep : %d\n", len_2);
    for (int i = 0; i < len_2; i++)
        printf("%s|", consecutive_sep[i]);
    printf("\nLen nothing_before_sep : %d\n", len_3);
    for (int i = 0; i < len_3; i++)
        printf("%s|", nothing_before_sep[i]);
    printf("\nLen nothing_after_sep : %d\n", len_4);
    for (int i = 0; i < len_4; i++)
        printf("%s|", nothing_after_sep[i]);
    printf("\nLen separators_only : %d\n", len_5);
    for (int i = 0; i < len_5; i++)
        printf("%s|", separators_only[i]);
    printf("\nLen separator_not_found : %d\n", len_6);
    for (int i = 0; i < len_6; i++)
        printf("%s|", separator_not_found[i]);
    printf("\nLen empty : %d\n", len_7);
    for (int i = 0; i < len_7; i++)
        printf("%s|", empty[i]);

    // Freeing memory
    free_arr(normal, len_1);
    free_arr(consecutive_sep, len_2);
    free_arr(nothing_before_sep, len_3);
    free_arr(nothing_after_sep, len_4);
    free_arr(separators_only, len_5);
    free_arr(separator_not_found, len_6);
    free_arr(empty, len_7);

    printf("\n\nFin des tests str_to_arr\n%s\n", sp);
}

/**
 * Tests functions for redirection
 */
void tests_redirection_functions() {
    printf("%sDébut des tests fonctions redirection\n\n", sp);

    // Strings to test
    char str1[11] = "cmd normal\0";
    char str2[16] = "cmd A > blabla \0";
    char str3[18] = "cmd B < blablabla\0";
    char str4[12] = "cmd C >    \0";
    char str5[8]  = "cmd D <\0";
    char str6[10] = "< nothing\0";
    char str7[2]  = ">\0";
    char str8[14] = "cmd > f1 < f2\0";
    char str9[10] = "cmd > a b\0";
    char str10[6] = "  > a\0";

    // occ_in_str(char *str, char c)
    int occ1  = occ_in_str(str1, '>'); // 0
    int occ2  = occ_in_str(str2, '>'); // 1
    int occ3  = occ_in_str(str3, '<'); // 1
    int occ4  = occ_in_str(str4, '>'); // 1
    int occ5  = occ_in_str(str5, '<'); // 1
    int occ6  = occ_in_str(str6, '<'); // 1
    int occ7  = occ_in_str(str7, '>'); // 1
    int occ8  = occ_in_str(str8, '>'); // 1
    int occ9  = occ_in_str(str8, '>'); // 1
    int occ10 = occ_in_str(str8, '>'); // 1

    printf("Tests occ_in_str() =\n\n");

    // Output tests occ_in_str()
    printf("Occurences de '%c' dans [%s] : %d\n", '>', str1, occ1);
    printf("Occurences de '%c' dans [%s] : %d\n", '>', str2, occ2);
    printf("Occurences de '%c' dans [%s] : %d\n", '<', str3, occ3);
    printf("Occurences de '%c' dans [%s] : %d\n", '>', str4, occ4);
    printf("Occurences de '%c' dans [%s] : %d\n", '<', str5, occ5);
    printf("Occurences de '%c' dans [%s] : %d\n", '<', str6, occ6);
    printf("Occurences de '%c' dans [%s] : %d\n", '>', str7, occ7);
    printf("Occurences de '%c' dans [%s] : %d\n", '>', str8, occ8);
    printf("Occurences de '%c' dans [%s] : %d\n", '>', str9, occ9);
    printf("Occurences de '%c' dans [%s] : %d\n", '>', str10, occ10);

    // occ_in_str(char *str, char c)
    int firstOcc1  = strFirstOccOfChar(str1, '>');  // -1
    int firstOcc2  = strFirstOccOfChar(str2, '>');  // 6
    int firstOcc3  = strFirstOccOfChar(str3, '<');  // 6
    int firstOcc4  = strFirstOccOfChar(str4, '>');  // 6
    int firstOcc5  = strFirstOccOfChar(str5, '<');  // 6
    int firstOcc6  = strFirstOccOfChar(str6, '<');  // 0
    int firstOcc7  = strFirstOccOfChar(str7, '>');  // 0
    int firstOcc8  = strFirstOccOfChar(str8, '>');  // 4
    int firstOcc9  = strFirstOccOfChar(str9, '>');  // 4
    int firstOcc10 = strFirstOccOfChar(str10, '>'); // 2

    printf("\nTests strFirstOccOfChar() =\n\n");

    // Output tests strFirstOccOfChar()
    printf("Première Occurence de '%c' dans [%s] : %d\n", '>', str1, firstOcc1);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '>', str2, firstOcc2);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '<', str3, firstOcc3);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '>', str4, firstOcc4);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '<', str5, firstOcc5);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '<', str6, firstOcc6);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '>', str7, firstOcc7);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '>', str8, firstOcc8);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '>', str9, firstOcc9);
    printf("Première Occurence de '%c' dans [%s] : %d\n", '>', str10, firstOcc10);

    // isValidRedirection(char *cmd)
    int valid1  = isValidRedirection(str1);  // 0
    int valid2  = isValidRedirection(str2);  // 1
    int valid3  = isValidRedirection(str3);  // 1
    int valid4  = isValidRedirection(str4);  // 0
    int valid5  = isValidRedirection(str5);  // 0
    int valid6  = isValidRedirection(str6);  // 0
    int valid7  = isValidRedirection(str7);  // 0
    int valid8  = isValidRedirection(str8);  // 0
    int valid9  = isValidRedirection(str9);  // 0
    int valid10 = isValidRedirection(str10); // 0

    printf("\nTests isValidRedirection() =\n\n");

    // Output tests isValidRedirection()
    printf("Test redirection commande pour [%s] : %d\n", str1, valid1);
    printf("Test redirection commande pour [%s] : %d\n", str2, valid2);
    printf("Test redirection commande pour [%s] : %d\n", str3, valid3);
    printf("Test redirection commande pour [%s] : %d\n", str4, valid4);
    printf("Test redirection commande pour [%s] : %d\n", str5, valid5);
    printf("Test redirection commande pour [%s] : %d\n", str6, valid6);
    printf("Test redirection commande pour [%s] : %d\n", str7, valid7);
    printf("Test redirection commande pour [%s] : %d\n", str8, valid8);
    printf("Test redirection commande pour [%s] : %d\n", str9, valid9);
    printf("Test redirection commande pour [%s] : %d\n", str10, valid10);

    printf("\n\nFin des tests fonctions redirection\n%s\n", sp);
}

/**
 * Multiple functions tests
 * outputs nor very clean
 */
void random_tests() {
    printf("%sDébut des tests random_tests\n\n", sp);

    char pwd_has_tar[100] = "/home.t/chang/Diderot/L3.tar/SY5./systl3_projet_gr_16.ta";
    char pwd_no_tar[100] = "/home/chang/Diderot/L3/SY5/systl3_projet_gr_16";

    // Few tests for has_tar_in_path();
    if (has_tar_in_path(pwd_has_tar))
        printf("PWD has a .tar\n");
    if (has_tar_in_path(pwd_no_tar) == 0)
        printf("NOT FOUND .tar\n");

    printf("-----------\n");

    // Tests for get_path_length();
    char len_1[10] = "/home";
    char len_1_2[10] = "home";
    char len2[20] = "//home////a///";
    char len3[20] = "home a    b ";
    char len3_2[20] = "home a    b/";

    printf("Length pwd = %d, %s\n", get_path_length(pwd_has_tar, '/'), remove_duplicate_sep(pwd_has_tar, '/'));
    printf("Length 1 = %d, %s\n", get_path_length(len_1, '/'), remove_duplicate_sep(len_1, '/'));
    printf("Length 1 = %d, %s\n", get_path_length(len_1_2, '/'), remove_duplicate_sep(len_1_2, '/'));
    printf("Length 2 = %d, %s\n", get_path_length(len2, '/'), remove_duplicate_sep(len2, '/'));
    printf("Length 3 = %d, %s\n", get_path_length(len3, ' '), remove_duplicate_sep(len3, ' '));
    printf("Length 3 = %d, %s\n", get_path_length(len3_2, ' '), remove_duplicate_sep(len3_2, ' '));

    printf("-----------\n");

    // Tests for remove_duplicate_sep()
    char **str_array = str_to_arr(pwd_has_tar, '/');
    char **str_array_mul = str_to_arr(len2, '/');
    int l1 = get_path_length(pwd_has_tar, '/');
    int l2 = get_path_length(len2, '/');

    for (int i = 0; i < l1; i++)
        printf("Arr[%d] : %s\n", i, str_array[i]);

    printf("-----------\n");

    for (int i = 0; i < l2; i++)
        printf("Arr[%d] : %s\n", i, str_array_mul[i]);

    printf("-----------\n");

    // Tests for get path name of : before tar/ tar/ after tar
    char *tar_name = get_tar_only(pwd_has_tar, '/');
    char *path_before_tar = get_path_before_tar(pwd_has_tar, '/');
    char *path_after_tar = get_path_after_tar(pwd_has_tar);
    printf("FULL PATH RECONSTRUCTED : %s\n%s\n%s\n", path_before_tar, tar_name, path_after_tar);

    printf("-----------\n");

    // Test get_last_of_path()
    char *last = get_last_of_path(pwd_has_tar, '/');
    printf("Last Of Path : %s\n", last);

    // Free
    free_arr(str_array, 6);
    free_arr(str_array_mul, 2);
    free(tar_name);
    free(path_before_tar);
    free(path_after_tar);
    free(last);

    // Test get_first_after_path
    printf("\nget_first_after_path :");
    char *path1 = "/home.t/chang/Diderot/L3.tar/SY5";
    char *path2 = "/home.t/chang/Diderot/L3.tar/SY5/popito";
    printf("%s\n", get_first_after_path(path1, path2, '/'));

    printf("\nremove All Ooccurence c from the beginning:");
    printf("\n%s", removeAllOcAtBeginning("000202", '0'));

    printf("\n\nconvert unix time:");
    printf("\n%s", convertUnixTime("13731732500"));

    printf("\n\nFin des tests random_tests\n%s\n", sp);
}

 // Tests
int main(int argc, char const *argv[])
{
    tests_get_path_until_tar();
    tests_str_to_arr();
    tests_redirection_functions();
    random_tests();

    return 0;
}
