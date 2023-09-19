#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <regex>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ctime>
#include <Windows.h>
#include <locale.h>

using namespace std;



void wait(int time) {
    this_thread::sleep_for(std::chrono::milliseconds(time));
}

// Esta função copia uma string para a área de transferência (clipboard) do Windows.
void copy_clipboard(const string& txt_for_output) {

    // Abre a área de transferência.
    if (OpenClipboard(nullptr)) {
        // Limpa qualquer dado existente na área de transferência.
        EmptyClipboard();

        // Aloca memória global para armazenar a string.
        HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, txt_for_output.size() + 1);

        // Verifica se a alocação de memória foi bem-sucedida.
        if (hClipboardData) {
            // Bloqueia a memória global para permitir a gravação de dados.
            char* pchData = (char*)GlobalLock(hClipboardData);

            // Copia a string para a memória global.
            strcpy(pchData, txt_for_output.c_str());

            // Desbloqueia a memória global.
            GlobalUnlock(hClipboardData);

            // Define o formato dos dados na área de transferência como texto simples (CF_TEXT).
            SetClipboardData(CF_TEXT, hClipboardData);

        }

        cout << "\033[32mO texto foi copiado para Area de transferencia!\033[0m\n\n";
        wait(2000);

        // Fecha a área de transferência.
        CloseClipboard();
    }
}

void open_url(const string& url, string type, string item_ip){

    wstring wideUrl(url.begin(), url.end()); // Converte a string C++ para LPCWSTR

    HINSTANCE result = ShellExecuteW(NULL, L"open", wideUrl.c_str(), NULL, NULL, SW_SHOWNORMAL); //Essa linha de código executa a função ShellExecute, que é usada para abrir uma URL em um navegador padrão no Windows.:

    if ((intptr_t)result <= 32) {
        // Um valor de retorno menor ou igual a 32 indica um erro.
        // Você pode tratar os erros aqui, se desejar.
        // Por exemplo:
        if ((intptr_t)result == ERROR_FILE_NOT_FOUND) {
             cout << "URL não encontrada" << endl;
        } else {
             cout << "URL encontrada" << endl;
        }
    }
}

string pickhour() {
    // Obtém o horário atual
    chrono::system_clock::time_point now = chrono::system_clock::now();

    // Converte o horário atual para um formato legível
    time_t currentTime = chrono::system_clock::to_time_t(now);

    // Extrai e formata apenas o horário
    tm* timeinfo = localtime(&currentTime);

    // Armazena o horario na variavel string hour
    ostringstream formattedTime;
    formattedTime << put_time(timeinfo, "%H:%M");
    string hour = formattedTime.str();

    return hour;
}

string getanswer() {

    string answer;
    getline(cin, answer);
    wait(550);

    return answer;
}

string msg_based_on_hour() {

    string hour_msg;

    string hour = pickhour();
    regex morning("[0-1][6-9-0-1]\\:[0-5][0-9]");
    regex afternoon("[0-1][2-7]\\:[0-5][0-9]");
    regex evening("^(1[8-9])?(2[0-3])?\\:[0-5][0-9]");

    if (regex_search(hour, morning)) {
        hour_msg = "Bom dia! Segue o checklist:";
    }
    if (regex_search(hour, afternoon)) {
        hour_msg = "Boa tarde! Segue o checklist:";
    }
    if (regex_search(hour, evening)) {
        hour_msg = "Boa noite! Segue o checklist:";
    }

    return hour_msg;

}

void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    cout << "*****************************\n";
    cout << "*** \x1B[35mAZReports\x1B[0m - Checklist ***\n";
    cout << "*****************************\n";
}

vector<string> txt_vetorize(string txt_file_name) {

    fstream input_txt(txt_file_name);

    if (input_txt.is_open()) {
        //cout << "O arquivo foi importado corretamente, prosseguindo." << endl;

        vector<string> txt_vectorized;
        string actual_line;

        while (getline(input_txt, actual_line)) {
            txt_vectorized.push_back(actual_line);
        }
        //cout << "linhas do arquivo txt foram vetorizadas com sucesso, prosseguindo." << endl;
        return txt_vectorized;
    }
    else {
        cout << "O arquivo " << txt_file_name << " nao foi encontrado! Encerrando." << endl;
        exit(0);
    }

}

string location_more(string loc) {

    string var_return;

    vector <string> wiki_locations = txt_vetorize("wiki_locations.txt");
    regex rgx_loc("([A-Za-z]+)\\s+(.*)");
    smatch smatch_loc;

    for (string line : wiki_locations) {
        regex_search(line, smatch_loc, rgx_loc);

        if (loc == smatch_loc[1]) {
            var_return = smatch_loc[2];
        }
        if (loc == smatch_loc[2]) {
            var_return = smatch_loc[1];
        }

    }
    return var_return;
}

vector<string> detect_item_type_single(string item) {

    string var_return, vmware_machine;
    vector <string> wiki_units = txt_vetorize("wiki_units.txt");
    vector <string> wiki_vmwares = txt_vetorize("wiki_vmwares.txt");
    vector <string> wiki_serverlists = txt_vetorize("wiki_serverlist.txt");

    regex rgx_line_unit("([A-Za-z]+[0-9]+[A-Za-z]+)\\s([0-9]+.[0-9]+.[0-9]+.[0-9]+)\\s(.*)");
    regex rgx_line_vmware("([0-9]+)\\s([A-Za-z]+[0-9]+[A-Za-z]+)\\s([A-Za-z]+\\_[A-Za-z]+\\_[A-Za-z]+[0-9]+)\\s([A-Za-z]+)\\s([0-9]+.[0-9]+.[0-9]+.[0-9]+)");
    regex rgx_line_serverlist("([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)\\s+([A-Za-z]+[0-9]+[A-Za-z]+)\\s(.*)\\s?(\\d{4})?");
    
    smatch smatch_wiki_units, smatch_wiki_vmwares, smatch_wiki_serverlists;

    regex rgx_machine("[A-Z-a-z-Á-Ú-á-ú]+\\s([0-9]+)$");
    smatch smatch_machine;

    //vectors para exportação das informações //vec return para juntar as 2 vectors no return da função
    vector<string> vec_unit, vec_vmware, vec_serverlists, vec_return;

    int count;

    //separar conteudo de cada linha de unidade (nome, ip, local) depois comparar o nome que foi passado com o nome da unidade
    count = 0;
    for (string line : wiki_units) {

        regex_search(line, smatch_wiki_units, rgx_line_unit);
        string unit_name = smatch_wiki_units[1];
        string unit_ip = smatch_wiki_units[2];
        string unit_loc = smatch_wiki_units[3];

        string full_loc = location_more(unit_loc);

        //se o usuario passou o nome / ip da unidade no cin
        if ((item == unit_name) || (item == unit_ip)) {

            var_return = "\x1B[32mUnidade\x1B[0m: " + unit_name + "\nIp: " + unit_ip + "\nLocalizacao: [" + full_loc + "] " + unit_loc + "\n";

            vec_unit.push_back(var_return);
            vec_unit.push_back(unit_name);
            vec_unit.push_back(unit_ip);
            vec_unit.push_back(unit_loc);
            vec_unit.push_back(full_loc);

            vec_return = vec_unit;

            break;
        }
        count++;
    }

    count = 0;
    for (string line : wiki_vmwares) {

        //BUG: fazer diferenciação de maiuscula e minuscula: ex: SRVHVM004RMM (user input) srvhvm004rmm (wiki item)

        //pegar a maquina e sair da condicao
        if (regex_search(line, smatch_machine, rgx_machine)) {
            vmware_machine = smatch_machine[1];
        }

        regex_search(line, smatch_wiki_vmwares, rgx_line_vmware);
        string vmware_port = smatch_wiki_vmwares[1];
        string vmware_name = smatch_wiki_vmwares[2];
        string vmware_container = smatch_wiki_vmwares[3];
        string vmware_loc = smatch_wiki_vmwares[4];
        string vmware_ip = smatch_wiki_vmwares[5];

        string full_loc = location_more(vmware_loc);

        if ((item == vmware_name) || (item == vmware_ip) || (item == vmware_container)) {

            var_return = "\x1B[36mVMWare\x1B[0m: " + vmware_name + "\nMaquina: " + vmware_machine + "\nContainer name: " + vmware_container + "\nip: " + vmware_ip + "\nPorta: " + vmware_port + "\nLocalizacao: [" + vmware_loc + "] " + full_loc + "\n";

            vec_vmware.push_back(var_return);
            vec_vmware.push_back(vmware_name);
            vec_vmware.push_back(vmware_machine);
            vec_vmware.push_back(vmware_container);
            vec_vmware.push_back(vmware_ip);
            vec_vmware.push_back(vmware_port);
            vec_vmware.push_back(vmware_loc);
            vec_vmware.push_back(full_loc);

            vec_return = vec_vmware;

            break;
        }
        count++;
    }
    
    for (string line : wiki_serverlists) {

        regex_search(line, smatch_wiki_serverlists, rgx_line_serverlist);
        string serverlists_ip = smatch_wiki_serverlists[1];
        string serverlists_host = smatch_wiki_serverlists[2];
        string serverlists_descr = smatch_wiki_serverlists[3];
        string serverlists_port = smatch_wiki_serverlists[4];

        //se o usuario passou o nome / ip da unidade no cin
        if ((item == serverlists_ip) || (item == serverlists_host)) {

            var_return = "\x1B[35mServidor (Host)\x1B[0m: " + serverlists_host + "\nIp: " + serverlists_ip + "\nPorta: " + serverlists_port + "\nDescricao: " + serverlists_descr + "\n";

            vec_serverlists.push_back(var_return);
            vec_serverlists.push_back(serverlists_host);
            vec_serverlists.push_back(serverlists_ip);
            vec_serverlists.push_back(serverlists_port);
            vec_serverlists.push_back(serverlists_descr);

            vec_return = vec_serverlists;

            break;
        }
        count++;
    }

    return vec_return;

}

string pick_path(string item_path, string item_name){

    vector<string> pathes = txt_vetorize(item_path);
    if(item_name == "PuTTY"){
        return pathes[0];
    }

    return "ERROR";
}

vector<string> performance_questions(string received_item, string item_type, string item_ip) {

    regex rgx_choices("[" + received_item + "]"); //receber a resposta por exemplo '234' e colocar entre [234]

    string resp_acessossh, resp_acessoui, string_uiline;
    vector<string> mensagens;

    string putty_path = pick_path("item_path.txt", "PuTTY") + "\\putty.exe"; //abrir arquivo com o caminho do PuTTY do usuario para depois utilizar o mesmo para abrir

    if (regex_search("1", rgx_choices)) {

        mensagens.push_back("sem trazer metricas");

        if(regex_search(item_type, regex("Unidade"))){

            while(true){

                cout << "\nJa tentou fazer o acesso SSH?";
                cout << "\n[1] Sim, e consegui acesso";
                cout << "\n[2] Tentei fazer mas nao consegui acesso";
                cout << "\n\n#Acoes SSH";
                cout << "\n[3] Copiar IP";
                cout << "\n[4] Copiar IP e abrir PuTTY\n\n> ";
                
                resp_acessossh = getanswer();

                if (resp_acessossh == "1") {
                    mensagens.push_back("com acesso ssh");
                    break;
                }

                if (resp_acessossh == "2") {
                    mensagens.push_back("sem acesso SSH");
                    break;
                }

                if ((resp_acessossh == "3")||(resp_acessossh == "4")) {
                    copy_clipboard(item_ip);
                }

                if (resp_acessossh == "4") {
                    open_url(putty_path, "PuTTY", item_ip);
                }
            }
            
        }

        if(regex_search(item_type, regex("VMWare"))){

            //o loop serve para que o item 3 não faça sair da pergunta, ou seja so ira sair quando a reposta for 1 ou 2.
            while(true){
                cout << "Ja tentou fazer o acesso ui no navegador?";
                cout << "\n[1] Sim, e consegui acesso";
                cout << "\n[2] Tentei fazer mas nao consegui acesso";
                cout << "\n\n#Acoes de acesso /ui";
                cout << "\n[3] Gerar <ip>/ui e copiar para o Clipboard";
                cout << "\n[4] Gerar <ip>/ui e abrir no navegador\n> ";

                resp_acessoui = getanswer();
                if (resp_acessoui == "1") {
                    mensagens.push_back("com acesso no /ui");
                    break;
                }
                if (resp_acessoui == "2") {
                    mensagens.push_back("sem acesso no /ui");
                    break;
                }
                if (resp_acessoui == "3") {
                    string_uiline = item_ip + "/ui";
                    copy_clipboard(string_uiline);
                }
                if (resp_acessoui == "4") {
                    string_uiline = "https://" + item_ip + "/ui";
                    open_url(string_uiline, "link/ui", "");
                }
            }
            
        }
    }
    if (regex_search("2", rgx_choices)) {
        mensagens.push_back("alto consumo de CPU");
    }
    if (regex_search("3", rgx_choices)) {
        mensagens.push_back("alto consumo de Memoria RAM");
    }
    if (regex_search("4", rgx_choices)) {
        mensagens.push_back("alto consumo de Disco");
    }
    if (regex_search("5", rgx_choices)) {
        mensagens.push_back("memoria swap alta");
    }
    if (regex_search("6", rgx_choices)) {
        mensagens.push_back("sysload alto");
    }
    if (regex_search("7", rgx_choices)) {
        mensagens.push_back("temperatura alta");
    }

    return mensagens;

}

string other_systems_questions(string received_item){

    string resp_cartoes, resp_cartoes_hour, resp_inventarios, resp_inventarios_which, resp_mastersaf, resp_ordensdservico, resp_ordensdservico_time, resp_elastic, resp_elastic_clusters, resp_elastic_services, resp_elastic_traces, resp_elastic_stream;
    regex rgx_choices("[" + received_item + "]"); //receber a resposta por exemplo '234' e colocar entre [234]
    vector<string> mensagens;

    string elastic_incidents; //variavel geral de todos os problemas reunidos.

    if(regex_search("1", rgx_choices)){

    cout << "\nA quanto tempo os cartoes nao coletam?";
    cout << "\n[1] Nao coletaram hoje. ";
    cout << "\n[2] Especificar tempo da ultima coleta\n> ";

    resp_cartoes = getanswer();

        if(resp_cartoes == "1"){
            mensagens.push_back("Cartoes nao coletaram hoje.");
        }

        if(resp_cartoes == "2"){
            cout << "\nDigite [DD/MM as HH:MM] (Dia Mês Hora e Minuto.)\n> ";
            resp_cartoes_hour = getanswer();
            mensagens.push_back("Cartoes nao coletam desde: " + resp_cartoes_hour);
        }
    }

    if(regex_search("2", rgx_choices)){
        //problemas relacionados a inventarios.

        if(resp_inventarios == "2"){
            cout << "\nQual aba de inventario esta com problema?\n[1] Aba InfraOps\n[2] Aba CloudOps\n> " << endl;
            resp_inventarios_which = getanswer();

            if(regex_search(resp_inventarios_which, regex("^1$"))){
                mensagens.push_back("Inventarios: Aba InfraOps nao esta trazendo metricas.");
            }
            if(regex_search(resp_inventarios_which, regex("^2$"))){
                mensagens.push_back("Inventarios: Aba CloudOps nao esta trazendo metricas.");
            }
            if(regex_search(resp_inventarios_which, regex("\\d{2}"))){
                mensagens.push_back("Inventarios: Abas CloudOps e InfraOps nao estao trazendo metricas.");
            }

        }

    }

    if(regex_search("3", rgx_choices)){
        //problemas relacionados a Mastersaf - WEB (Up ou Down)

        cout << "\nQuais das abas estao down?";
        cout << "\n[1] OneSource PRD";
        cout << "\n[2] DFE PRD\n> ";

        resp_mastersaf = getanswer();

        if(regex_search(resp_mastersaf, regex("^1$"))){
            mensagens.push_back("Mastersaf: Aba OneSource esta down.");
        }
        if(regex_search(resp_mastersaf, regex("^2$"))){
            mensagens.push_back("Mastersaf: Aba DFE PRD esta down.");
        }
        if(regex_search(resp_mastersaf, regex("\\d{2}"))){
            mensagens.push_back("Mastersaf: Abas OneSource PRD e DFE PRD estao down.");
        }
        
    }

    if(regex_search("4", rgx_choices)){
        //problemas relacionados a ordens de serviço

        cout << "\nQuais problemas estao ocorrendo com as Ordens de serviço?";
        cout << "\n[1] Ordem de serviço com mais de 40min";
        cout << "" << endl;
        cout << "\n[2] Kollector Status nao esta Running\n> ";

        resp_ordensdservico = getanswer();

        if(resp_ordensdservico == "1"){
            mensagens.push_back("Identificamos as seguintes ordens passando dos 40min:");
        }
        if(resp_ordensdservico == "2"){
            mensagens.push_back("Kollector Status nao esta Running");
        }

    }

    if(regex_search("5", rgx_choices)){
        //problemas relacionados ao ELASTIC

        vector<string> elastic_problems;

        string elastic_initial_msg;
        string elastic_quit_msg;
        string elastic_incidents, elastic_area_saved, elastic_area = "";
        bool elastic_first_time = true;

        while(true){
            
            if(elastic_first_time == true){
                elastic_first_time = false;
                elastic_quit_msg = "";
                elastic_initial_msg = "\nQual problema relacionado ao Elastic?";
            }
            else{
                elastic_quit_msg = "\n[7] Mais nenhum problema relacionado ao Elastic.";
                elastic_initial_msg = "\nMais algum problema relacionado ao Elastic?";
            }

            cout << elastic_initial_msg;
            cout << "\n#Area do Elastic:";
            cout << "\n[1] Dev";
            cout << "\n[2] Prod";
            cout << "\n#Problemas:";
            cout << "\n[3] Clusters";
            cout << "\n[4] Services";
            cout << "\n[5] Traces";
            cout << "\n[6] Stream";
            cout << elastic_quit_msg << "\n>";

            resp_elastic = getanswer();

            //elastic_area // prod ou dev
            if(regex_search(resp_elastic, regex("1"))){
                elastic_area = "Elastic de DEV - ";
            }

            if(regex_search(resp_elastic, regex("2"))){
                elastic_area = "Elastic de PROD - ";
            }
            //

            if(regex_search(resp_elastic, regex("3"))){
                
                cout << "\nQual problema relacionado a cluster esta ocorrendo?\n[1] 'No monitoring data found'\n> ";
                resp_elastic_clusters = getanswer();

                if(resp_elastic_clusters == "1"){
                    elastic_problems.push_back("Aba cluster apresentando problemas na tentativa de acesso");
                }

            }

            if(regex_search(resp_elastic, regex("4"))){
                elastic_problems.push_back("Aba services sem trazer metricas.");
            }

            if(regex_search(resp_elastic, regex("5"))){
                elastic_problems.push_back("Aba traces sem trazer metricas.");
            }

            if(regex_search(resp_elastic, regex("6"))){
                
                cout << "\nQual o problema relacionado a aba Stream?\n[1] Sem trazer logs a X tempo\n> ";
                resp_elastic_stream = getanswer();

                if(resp_elastic_stream == "1"){
                    cout << "\nDigite o momento que a ultima log chegou [DD:MM / HH:MM] :\n> ";
                    string resp_time = getanswer();
                    elastic_problems.push_back("Aba stream sem trazer logs, ultima log recebida as: " + resp_time);
                }

            }

            if(regex_search(resp_elastic, regex("7"))){
                cout << "\nCerto, finalizando o report do Elastic.\n";
                return elastic_incidents;
            }

            //concaternar incidentes do elastic lado a lado, com a area no inicio (prod ou dev.)
            int elastic_problems_size = elastic_problems.size() -1;

            string elastic_stringmatch = "";
            string padding = " | ";
            int count = 0;

            for(string line : elastic_problems){

                // se for a ultima linha do vector nao precisaremos mais do | para separar.
                if(count == elastic_problems_size){
                    padding = "";
                }

                elastic_stringmatch = line + padding;

                count++;
            }
            
            elastic_incidents = elastic_incidents + elastic_area + elastic_stringmatch + "\n";

        }

    }

    return "abc";
}

string final_way_several(vector<vector<string>> all_receivers, bool last_item) {

    string receiver_ip_swap, last_return;
    string msg_hour = msg_based_on_hour();

    //adicionar cada nome dos itens em um novo vector item_names para exibir.

    string item_type;
    string padding;
    string item_name_and_ip;
    string quebrar_linha;

    vector<string> std_msgs;

    int all_receivers_last_item;
    int receiver_last_item;
    int count = 0;
    for (vector<string> receiver : all_receivers) {

        all_receivers_last_item = all_receivers.size() - 1; //posição final do vetor all_receivers
        receiver_last_item = receiver.size() - 1; //posição final do vetor receivers

        if (regex_search(receiver[0], regex("Unidade"))) {
            item_type = "Unidade";
            receiver_ip_swap = receiver[2];
            padding = " | ";
            std_msgs.push_back(" ");
            std_msgs.push_back(" ip: ");
        }
        if (regex_search(receiver[0], regex("VMWare"))) {
            item_type = "VMWare";
            receiver_ip_swap = receiver[4];
            padding = " | ";
            std_msgs.push_back(" ");
            std_msgs.push_back(" ip: ");
        }
        if (regex_search(receiver[0], regex("(?:M.tricas)?(?:Unidades)?.*normalmente."))) {
            item_type = "";
            receiver_ip_swap = "";
            padding = "";
            std_msgs.push_back("");
            std_msgs.push_back("");
            receiver.insert(receiver.begin(),""); //fazer com que o item 0 de receiver vire 1, adicionando ao inicio (0) qualquer conteudo..
        }

        if (regex_search(receiver[0], regex("Elastic"))) {
            item_type = "";
            receiver_ip_swap = "";
            padding = "";
            std_msgs.clear(); //limpar vector dos itens anteriores.
            std_msgs.push_back("");
            std_msgs.push_back("");
            receiver.insert(receiver.begin(),""); //fazer com que o item 0 de receiver vire 1, adicionando ao inicio (0) qualquer conteudo.. assim ele ira ser pego la em baixo na variavel.
        }

        //se não for a primeira vez
        if (count > 0) {
            quebrar_linha = "\n";
        }

        item_name_and_ip = item_name_and_ip
        + quebrar_linha
        + item_type
        + std_msgs[0]
        + receiver[1]
        + std_msgs[1]
        + receiver_ip_swap
        + padding
        + receiver[receiver_last_item]
        + std_msgs[0];

        count++;
    }

    //

    last_return =
        "**CHECKLIST**\n\n"
        + msg_hour  //bom dia, segue o checklist:
        + "\n\n"    //quebra de linha dupla
        + item_name_and_ip;

    if(last_item == true){
        copy_clipboard(last_return);
    }

    return last_return;

}

string detect_type(string receiver_zero) {

    string item_type;

    //detectar se é uma unidade ou vmware //fazer função depois.
    if (regex_search(receiver_zero, regex("Unidade"))) {
        item_type = "Unidade";
    }
    if (regex_search(receiver_zero, regex("VMWare"))) {
        item_type = "VMWare";
    }
    //

    return item_type;

}

int main() {

    setlocale(LC_ALL, "pt_BR.utf8");

    string item, first_q, subfirst_q, incident_type, full_incidents, backup_last_full_incidents, item_ip;
    vector<vector<string>> all_receivers;
    vector<vector<string>> bkp_all_receivers;

    int count = 0;

    bool first_quest = true; //var que se ativa de cara, para a pergunta ser feita de primeira
    bool second_report = false;
    bool last_item = false;

    while (true) {

        vector<string> items;
        vector<string> receiver;

        regex rgx_comma_items("([^,\\s+]+)"); //regex para coletar tudo que não seja virgula e espaços '^,\\s+'

        smatch smatch_comma_items;

        string item_type, use_report;

        if (first_quest == true) {
            
            limpar_tela();

            cout << "\n[1] Gerar report normalizado\n[2] Analisar um ou mais itens\n[3] Analisar e Reportar um ou mais itens\n[4] Outros Reports (Elastic, Cartoes e etc)\n[5] Encerrar programa\n\n> ";
            first_q = getanswer();
            //transforma a resposda do usuario em uma regex para usar como comparação abaixo

        }

        if (regex_search(first_q, regex("[1]"))) {

            cout << "\nDe qual empresa deseja gerar o report normalizado?";
            cout << "\n\n[1] BRK Ambiental";
            cout << "\n[2] Qualy System";
            cout << "\n[3] Data System";
            cout << "\n[4] Odontoprev\n\n> ";

            subfirst_q = getanswer();
            
            if(subfirst_q == "1"){
                receiver.push_back("Unidades, VMwares e Sistemas Linux estao funcionando normalmente.");
            }

            if(subfirst_q == "2"){
                receiver.push_back("Métricas da Qualy System estão sendo coletadas normalmente.");
            }
            
            if(subfirst_q == "3"){
                receiver.push_back("Métricas da DataSystem estão sendo coletadas normalmente.");
            }

            if(subfirst_q == "4"){
                receiver.push_back("Métricas da Odontoprev estão sendo coletadas normalmente.");
            }

            all_receivers.push_back(receiver);
            final_way_several(all_receivers, true);
            
        }

        if(regex_search(first_q, regex("[23]"))){

            cout << "\nDigite o item a ser analisado (\x1B[32mUnidade\x1B[0m, \x1B[36mVMWare\x1B[0m, IP, container_name):\n\n> ";
            item = getanswer();

            //usuario podera colocar 1 ou mais itens, separados por virgula.
            // 
            //codigo do diabo abaixo, um dia eu entenderei...
            //basicamente o que ele faz e pegar a entrada do usuario dividida por virgulas (usando a regex acima) e sempre
            //retornando cada captura no smatch[1], então usei isso para dar pushback.

            string::const_iterator searchStart(item.cbegin());
            while (regex_search(searchStart, item.cend(), smatch_comma_items, rgx_comma_items)) {
                searchStart = smatch_comma_items.suffix().first;
                items.push_back(smatch_comma_items[1]);
            }
            //

            count = 0;
            for (string current_item : items) {

                int items_size = items.size() -1;
                last_item = false;
                
                if(items_size == count){
                    last_item = true;
                }

                //limpar_tela();
                //primeiro crio o vetor receiver com os dados do item, depois adiciono ele a ultima linha do vetor de vetores
                vector<string> receiver = detect_item_type_single(current_item);
                all_receivers.push_back(receiver);

                //fiz com que a funcao detect_item_type_single retorne o conjunto da mensagem em '0' do vector e exibir os dados do item na tela de uma vez. para facilitar.
                limpar_tela();
                cout << "\nInfomacoes do item:\n" << endl << receiver[0] << endl;

                //neste codigo for, "receiver" sera o ultimo vetor listado em all_receivers.]
                //uso meio que pra deixar o item anterior "guardado" e sempre usar o ultimo para mecher.
                for (vector<string> actual_vector : all_receivers) {
                    receiver = actual_vector;
                }
                
                //detectar se é Unidade ou VMWare
                item_type = detect_type(receiver[0]);

                //pegar ip do item
                if(item_type == "Unidade"){
                    item_ip = receiver[2];
                }
                if(item_type == "VMWare"){
                    item_ip = receiver[4];
                }

                //ambas variaveis abaixo são vector<string>
                //exibir todos os itens para o usuario no terminal passados pelo usuario na entrada e destacar o atual
                string itens_show;
                string current_item_colored;
                int already_checked = 0;

                int i = 0;
                for (string actual_item : items) {
                    if (actual_item != current_item) {
                        itens_show = itens_show + actual_item + " ";
                    }
                    else {
                        itens_show = itens_show + "\033[33m" + actual_item + "\033[0m" + " ";
                        already_checked = i;
                    }
                    i++;
                }
                cout << "\nLista com todos os itens: " << itens_show << endl;

                //mudar a cor do item ja verificado para verde usando 'i' do for.
                items[already_checked] = "\033[32m" + items[already_checked] + "\033[0m";

                //

                if(regex_search(first_q,regex("[2]"))){
                    cout << "\n*****************************\nPressione Enter para ver o proximo item" << endl;
                    cin.get();
                    wait(500);
                }

                if(regex_search(first_q,regex("[3]"))){

                    if (second_report == true) {
                        use_report = "\n[4] usar report do item anterior.";
                    }
                    else {
                        use_report = "";
                    }

                    //até aqui o ultimo vector de  all_receivers ja foi preenchido com as informações
                    //e também já sabemos qual o tipo do item, VM ou unidade

                    cout << "\nO que deseja fazer referente a " << item_type << " " << receiver[1] << " agora?:\n\n";
                    cout  << "\033[33m[1] Criar novo report\033[0m\n[2] Copiar Hostname\n[3] Copiar IP\n[4] Copiar localizacao" << use_report << "\n\n> ";

                    item = getanswer();

                    if (item == "1") {

                        second_report = true; //essa var serve para fazer a opção 4 só depois que o primeiro report for realizado.

                        cout << "\nQual foi o tipo de incidente? \n" << "[1] Queda\n[2] Consumo de CPU\n[3] Consumo de Memoria\n[4] Consumo de Disco\n[5] Memoria Swap\n[6] Sysload\n[7] Temperatura\n\n> ";
                        item = getanswer();

                        //vector que ira ser preenchido com as respostas de desempenho, cpu, memoria etc
                        //depois iremos criar uma string concaternando as informações no for abaixo

                        vector<string> perf_questions = performance_questions(item, item_type, item_ip);

                        for (string line : perf_questions) {

                            //mensagens relacionadas a performance, concaternar todos os reports na string.

                            if (!line.empty()) { //se line conter um report, adicione ele a string

                                if (full_incidents.empty()) { //verificar se a string esta vazia, se sim adicionar sem espaçamento.

                                    full_incidents = line;
                                }
                                else {
                                    full_incidents = full_incidents + " | " + line;

                                }

                            }

                        }                    

                        //depois de adicionar o conteudo de identidade do item, iremos adicionar também seus incidentes no vetor de vetores 'all_receivers'
                        //sub vetor "count" //tambem passaremos se estamos na ultima ocorrencia do for
                        all_receivers[count].push_back(full_incidents);
                        bkp_all_receivers = all_receivers; //backup do vector all_receivers antes de passar pelo reset.

                        final_way_several(all_receivers, last_item);
                        
                        backup_last_full_incidents = full_incidents; //antes de resetar full_incidents, deixar um backup para usar no 4 (mesmos incidentes)
                        full_incidents = ""; //depois de incluir os incidentes na vector, resetar var para novos.

                        if((last_item == true) && regex_match(first_q, regex("[4]"))){ //se for o ultimo item do checklist & tiver problemas de elastic, sair do loop
                            cout << "regex last item break" << endl;
                            break;
                        }

                    }

                    if (item == "2"){

                    }
                    if (item == "3"){

                    }
                    if (item == "4"){

                    }
                    if (item == "5"){

                    }


                }

                if (item == "4") {
                    limpar_tela();

                    //utilizar backup da ultima lista de incidents para repetir no item desejado
                    all_receivers[count].push_back(backup_last_full_incidents); //adicionar ao novo report os itens de incident do anterior
                    bkp_all_receivers = all_receivers; //backup do vector all_receivers novamente, para ser usado caso tenha os outros incidentes, elastic... etc..
                    final_way_several(all_receivers, true);
                }

                count++;
            }
        }

        if(regex_search(first_q, regex("[4]")) && (last_item == true) || (regex_search(first_q, regex("^4$")))){
    
            //a ideia e que este report fique por ultimo de todos. por isso usamos last_item.
            cout << "\nQuais outros problemas estao ocorrendo?\n[1] Coleta de cartoes\n[2] Inventarios\n[3] Mastersaf - WEB\n[4] CRM Ordens de servico\n[5] Elastic\n" << endl;
            subfirst_q = getanswer();

            string ot_sys_questions = other_systems_questions(subfirst_q);

            //salvar string com incidentes do elastic e chamar final way

            vector<string> vec_other_questions;
            vec_other_questions.push_back(ot_sys_questions); //adicionar string de other_questions ao vetor que sera colocado no final do vetor de vetores.
            
            bkp_all_receivers.push_back(vec_other_questions);
            final_way_several(bkp_all_receivers, true);

        }

        if(regex_search(first_q, regex("[5]")) && (last_item == true)){
            cout << "\nObrigado por utilizar o AZReports! Finalizando.\n";
            exit(0);
        }
    }

    return 0;
}