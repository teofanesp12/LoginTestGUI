/*****************************************************************************
*                                                                            *
*  Copyright (C) 2015 New Company Ltda (<http://www.newcompany.com.br>).     *
*                                                                            *
*  Author Teofanes Enrique <teofanesp12@gmail.com.br>                        *
*                                                                            *
* Este programa é um software livre: você pode redistribuir e/ou modificar   *
* este programa sob os termos da licença GNU Affero General Public License,  *
* publicada pela Free Software Foundation, em sua versão 3 ou, de acordo     *
* com sua opção, qualquer versão posterior.                                  *
*                                                                            *
* Este programa é distribuido na esperança de que venha a ser útil,          *
* porém SEM QUAISQUER GARANTIAS, nem mesmo a garantia implícita de           *
* COMERCIABILIDADE ou ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Veja a          *
* GNU Affero General Public License para mais detalhes.                      *
*                                                                            *
* Você deve ter recebido uma cópia da GNU Affero General Public License      *
* juntamente com este programa. Caso esse não seja o caso, acesse:           *
* <http://www.gnu.org/licenses/>                                             *
*                                                                            *
* Este Software Foi Desenvolvido Para o Sistema GNU, atual kernel é o Linux  *
*                                                                            *
*****************************************************************************/

/* Tela de Login */
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <string.h>
typedef char* String;
/*Objeto de conexão*/
static PGconn *conn = NULL;
/*Ponteiro de resultado*/
static PGresult *result;

static GtkWidget 
    *password_entry, 
    *user_entry;


GtkInfoBar *bar;
GtkWidget *message_label;

GtkWidget* window;
static void
MsgBox(String txt, GtkWindow *window);

GtkGrid*
alert_create(char* msg);

void
alert(GtkMessageType message_type, char* msg, ...);

int
conexao(String dbName){
    String pghost="localhost",
           pgport="5432",
           pgoptions=NULL,
           pgtty=NULL,
           login=NULL,
           pwd=NULL;
    if (!dbName)
        dbName = "postgres";
    conn = PQsetdbLogin(pghost, pgport, pgoptions, pgtty, dbName, login, pwd);
    if(PQstatus(conn) == CONNECTION_OK){
        printf("Conexão efetuada com sucesso.\n");
        alert(GTK_MESSAGE_WARNING, "Conexão efetuada com sucesso.");
    }else{
        printf("Falha na conexão. Erro: %s\n", PQerrorMessage(conn));
        alert(GTK_MESSAGE_ERROR, "Falha na conexão");
        PQfinish(conn);
        return 1;
    }
    return 0;
}

void
exit_conexao(){
    /*Libera o nosso objeto*/
    if (result)
        PQclear(result);
    /*Verifica se a conexão está aberta e a encerra*/
    if(conn != NULL)
        PQfinish(conn);
}

static void
complet_combo_db(GtkWidget *combo_box){
    if (conexao(NULL))
        return;
    /*Executa o comando*/
    result = PQexec(conn, "select datname from pg_database where datistemplate=False;");
    
    if(!result)
    {
        printf("Erro executando comando. ");
    }
    else
    {
        switch(PQresultStatus(result))
        {
            case PGRES_EMPTY_QUERY:
                printf("Nada aconteceu.\n");
                break;
            case PGRES_TUPLES_OK:
                printf("A query retornou %d linhas.\n", PQntuples(result));
                break;
            case PGRES_FATAL_ERROR:
                printf("Erro na Consulta: %s\n", PQresultErrorMessage(result));
                break;
            case PGRES_COMMAND_OK:
                printf("%s linhas afetadas.\n", PQcmdTuples(result));
                break;
            default:
                printf("Algum outro resultado ocorreu.\n");
                break;
        }
    }
    
    /* Pegamos a Quantidade de campos capturados */
    int campos = PQnfields(result);
    int count  = PQntuples(result);
    
    int i,j;
    for(i = 0; i < count; i++)
    {
        // Percorre todas as colunas
        for (j = 0; j < campos; j++)
        {
            // Imprime o valor do campo
            gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), PQgetvalue(result, i, j));
        }
    }
    return exit_conexao();
}

static void
login_action (GtkButton *button,
                gpointer   user_data)
{
    const char *paramValues[2];
    
    int status_conn;
    String senha, usuario;
    senha   = (String) gtk_entry_get_text( GTK_ENTRY( password_entry ) );
    usuario = (String) gtk_entry_get_text( GTK_ENTRY( user_entry     ) );
    /* Verificamos se os campos Login e Senha estão preenchidos */
    if ( strcmp( usuario, "" ) == 0){
        alert(GTK_MESSAGE_WARNING, "O Campo Usuario Não foi Preenchido...");
        MsgBox("O Campo Usuario Não foi Preenchido...", GTK_WINDOW(window));
        return;
    }
    
    if ( strcmp( senha, "" ) == 0){
        alert(GTK_MESSAGE_WARNING, "O Campo Senha Não foi Preenchido...");
        MsgBox("O Campo Senha Não foi Preenchido...", GTK_WINDOW(window));
        return;
    }
    
    /* Agora Abrimos a conexao */
    String dbName = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(user_data));
    if (conexao(dbName))
        return;
    
    /*Executa o comando*/
    paramValues[0] = usuario;
    paramValues[1] = senha;

    result = PQexecParams(conn,
                       "SELECT count(*) FROM users WHERE login = $1",
                       1,       /* one param */
                       NULL,    /* let the backend deduce param type */
                       paramValues,
                       NULL,    /* don't need param lengths since text */
                       NULL,    /* default to all text params */
                       0);      /* ask for binary results */
    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        printf( "SELECT Falho: %s", PQerrorMessage(conn));
        exit_conexao();
        printf("O sistema Não Foi Instado na Base: %s\n",dbName);
        alert(GTK_MESSAGE_ERROR, g_strdup_printf( "O sistema Não Foi Instado na Base: \'%s\'.",dbName ));
        return;
    }
    
    /* Verificamos se o User Existe*/
    int num_records, i;
    num_records = PQntuples(result);
    for(i = 0 ; i < num_records ; i++)
    {
        if (strcmp(PQgetvalue(result, i, 0),"0" ) == 0){
            MsgBox("Usuario Não existe...", GTK_WINDOW(window));
            alert(GTK_MESSAGE_ERROR, "Usuario Não existe...");
            exit_conexao();
            return;
        }
    }
    
    /* Verificamos a Senha do Usuario */
    result = PQexecParams(conn,
                       "SELECT count(*) FROM users WHERE login = $1 and password = $2",
                       2,       /* two param */
                       NULL,    /* let the backend deduce param type */
                       paramValues,
                       NULL,    /* don't need param lengths since text */
                       NULL,    /* default to all text params */
                       0);      /* ask for binary results */
    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        printf( "SELECT failed: %s", PQerrorMessage(conn));
        exit_conexao();
    }
    num_records = PQntuples(result);
    for(i = 0 ; i < num_records ; i++)
    {
        if (strcmp(PQgetvalue(result, i, 0),"0" ) == 0){
            MsgBox("Senha Errada...", GTK_WINDOW(window));
            alert(GTK_MESSAGE_ERROR, "Senha Errada...");
            exit_conexao();
            return;
        }
    }
    exit_conexao();
}

static void
activate (GtkApplication *app, gpointer user_data){
    GtkWidget *button, *combo_db;
    GtkWidget *conteudo, *conteudo1, *conteudo2;
    GtkWidget *label_user, *label_password;
    /* Construimos a Janela */
    window = gtk_application_window_new (app);
    gtk_window_set_title(GTK_WINDOW(window), "Tela de Login");
    gtk_window_set_default_size (GTK_WINDOW (window), 100, 100);
    //gtk_widget_set_size_request(window, 100, 100);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    // Container Principal
    conteudo=gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), conteudo);
    
    gtk_container_add(GTK_CONTAINER(conteudo), GTK_WIDGET(alert_create("Teste ")) );
    
    // Area Do Banco de Dados
    combo_db = gtk_combo_box_text_new ();
    gtk_widget_set_tooltip_text(combo_db, "Banco de Dados.");
    complet_combo_db(combo_db);
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo_db), 0);
    gtk_container_add(GTK_CONTAINER(conteudo), combo_db);
    
    // Area do Usuario
    conteudo1=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(conteudo), conteudo1);
    label_user = gtk_label_new("Usuario: ");
    gtk_box_pack_start(GTK_BOX(conteudo1), label_user, FALSE, FALSE, 5);
    
    user_entry=gtk_entry_new();
    gtk_widget_set_tooltip_text(user_entry, "Usuario do Sistema.");
    gtk_entry_set_placeholder_text(GTK_ENTRY(user_entry), "Usuario");
    gtk_box_pack_start(GTK_BOX(conteudo1), user_entry, FALSE, FALSE, 2);
    
    // Area da Senha
    conteudo2=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(conteudo), conteudo2);
    label_password = gtk_label_new("Senha: ");
    gtk_box_pack_start(GTK_BOX(conteudo2), label_password, FALSE, FALSE, 5);
    
    password_entry=gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_widget_set_tooltip_text(password_entry, "Senha do Usuario do Sistema.");
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Senha");
    gtk_box_pack_start(GTK_BOX(conteudo2), password_entry, FALSE, FALSE, 14);
    
    // Inserir Botão
    button = gtk_button_new_with_label("Entrar");
    g_signal_connect (GTK_BUTTON (button),
                    "clicked", 
                    G_CALLBACK (login_action), 
                    G_OBJECT (combo_db));
    gtk_container_add(GTK_CONTAINER(conteudo), button);
    
    gtk_widget_show_all (window);
}

int
main(int argc, char **argv){
    GtkApplication *app;
    int status;
    app = gtk_application_new ("login.teste", G_APPLICATION_FLAGS_NONE);
    
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}

static void
on_response (GtkDialog *dialog,
             gint       response_id,
             gpointer   user_data)
{
  /*For demonstration purposes, this will show the int value 
  of the response type*/
  //g_print ("Resposta ao ID: %d\n", response_id);
  
  /*This will cause the dialog to be destroyed*/
  gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
MsgBox(String txt, GtkWindow *window){
    g_print("%s\n",txt);
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *label;

    gint response_id;

    /*Create the dialog window. Modal windows prevent interaction with other 
    windows in the same application*/
    dialog = gtk_dialog_new_with_buttons ("Aviso...", 
                                        window, 
                                        GTK_DIALOG_MODAL, 
                                        "_OK", 
                                        GTK_RESPONSE_OK, 
                                        NULL);

    /*Create a label and attach it to the content area of the dialog*/
    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    label = gtk_label_new (txt);
    gtk_container_add (GTK_CONTAINER (content_area), label);

    /*The main purpose of this is to show dialog's child widget, label*/
    gtk_widget_show_all (dialog);
    /*Connecting the "response" signal from the user to the associated
    callback function*/
    g_signal_connect (GTK_DIALOG (dialog), 
                    "response", 
                    G_CALLBACK (on_response), 
                    NULL);
}


GtkGrid*
alert_create(char* msg){
    GtkWidget *widget, *content_area;
    GtkGrid *grid = GTK_GRID(gtk_grid_new());

    widget = gtk_info_bar_new ();
    bar = GTK_INFO_BAR (widget);

    gtk_widget_set_no_show_all (widget, TRUE);
    message_label = gtk_label_new (msg);
    gtk_widget_show (message_label);
    content_area = gtk_info_bar_get_content_area (bar);
    gtk_container_add (GTK_CONTAINER (content_area),
                       message_label);

    gtk_info_bar_add_button (bar,
                             "_OK",
                             GTK_RESPONSE_OK);
    g_signal_connect (bar,
                      "response",
                      G_CALLBACK (gtk_widget_hide),
                      NULL);
    gtk_grid_attach (GTK_GRID (grid),
                     widget,
                     0, 2, 1, 1);

    // show an error message
    //gtk_label_set_text (GTK_LABEL (message_label), msg);

    //gtk_info_bar_set_message_type (bar, GTK_MESSAGE_ERROR);
    //gtk_info_bar_set_message_type (bar, GTK_MESSAGE_WARNING);
    //gtk_info_bar_set_message_type (bar, GTK_MESSAGE_QUESTION);
    gtk_info_bar_set_message_type (bar, GTK_MESSAGE_OTHER);

    gtk_widget_show (GTK_WIDGET(bar));
    return grid;
}

void
alert(GtkMessageType message_type, char* msg, ...){
    gtk_label_set_text (GTK_LABEL (message_label), msg);
    gtk_info_bar_set_message_type (bar, message_type);
    gtk_widget_show (GTK_WIDGET(bar));
}
