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

        // Fecha a área de transferência.
        CloseClipboard();
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
    wait(150);

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
    cout << "*** AZReports - Checklist ***\n";
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

    regex rgx_line_unit("([A-Za-z]+[0-9]+[A-Za-z]+)\\s([0-9]+.[0-9]+.[0-9]+.[0-9]+)\\s(.*)");
    regex rgx_line_vmware("([0-9]+)\\s([A-Za-z]+[0-9]+[A-Za-z]+)\\s([A-Za-z]+\\_[A-Za-z]+\\_[A-Za-z]+[0-9]+)\\s([A-Za-z]+)\\s([0-9]+.[0-9]+.[0-9]+.[0-9]+)");
    smatch smatch_wiki_units, smatch_wiki_vmwares;

    regex rgx_machine("[A-Z-a-z-Á-Ú-á-ú]+\\s([0-9]+)$");
    smatch smatch_machine;

    //vectors para exportação das informações //vec return para juntar as 2 vectors no return da função
    vector<string> vec_unit, vec_vmware, vec_return;

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

            var_return = "Unidade: " + unit_name + "\nIp: " + unit_ip + "\nLocalizacao: [" + full_loc + "] " + unit_loc + "\n";

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

            var_return = "VMWare: " + vmware_name + "\nMaquina: " + vmware_machine + "\nContainer name: " + vmware_container + "\nip: " + vmware_ip + "\nPorta: " + vmware_port + "\nLocalizacao: [" + vmware_loc + "] " + full_loc + "\n";

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

    return vec_return;

}

vector<string> performance_questions(string received_item) {

    regex rgx_choices("[" + received_item + "]"); //receber a resposta por exemplo '234' e colocar entre [234]

    string resp_acessossh, resp_acessoui;
    vector<string> mensagens;

    if (regex_search("1", rgx_choices)) {

        mensagens.push_back("sem trazer metricas");

        cout << "Ja tentou fazer o acesso SSH?\n[1] Sim, e consegui acesso\n[2] Tentei fazer mas nao consegui acesso\n[3] apenas informar ausencia de metricas\n> " << endl;
        resp_acessossh = getanswer();

        if (resp_acessossh == "1") {
            mensagens.push_back("com acesso ssh");
        }

        if (resp_acessossh == "2") {
            mensagens.push_back("sem acesso SSH");
        }

        cout << "Ja tentou fazer o acesso ui no navegador?\n[1] Sim, e consegui acesso\n[2] Tentei fazer mas nao consegui acesso\n[3] apenas informar ausencia de metricas\n> " << endl;
        resp_acessoui = getanswer();

        if (resp_acessoui == "1") {
            mensagens.push_back("com acesso no /ui");
        }

        if (resp_acessoui == "2") {
            mensagens.push_back("sem acesso no /ui");
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

string final_way_several(vector<vector<string>> all_receivers) {

    string receiver_ip_swap, last_return;
    string msg_hour = msg_based_on_hour();

    regex rgx_unit("Unidade:");
    regex rgx_vmwa("VMWare:");

    string item_type, padding;

    //adicionar cada nome dos itens em um novo vector item_names para exibir.

    string item_name_and_ip;
    string quebrar_linha;

    int all_receivers_last_item;
    int receiver_last_item;
    int count = 0;
    for (vector<string> receiver : all_receivers) {

        all_receivers_last_item = all_receivers.size() - 1; //posição final do vetor all_receivers
        receiver_last_item = receiver.size() - 1; //posição final do vetor receivers

        if (regex_search(receiver[0], rgx_unit)) {
            item_type = "Unidade";
            receiver_ip_swap = receiver[2];
        }
        if (regex_search(receiver[0], rgx_vmwa)) {
            item_type = "VMWare";
            receiver_ip_swap = receiver[4];
        }

        //se não for a primeira vez
        if (count > 0) {
            quebrar_linha = "\n";
        }
        padding = " | ";

        item_name_and_ip = item_name_and_ip + quebrar_linha + item_type + " " + receiver[1] + " ip: " + receiver_ip_swap + padding + receiver[receiver_last_item] + " ";

        count++;
    }

    //

    last_return =
        "**CHECKLIST**\n\n"
        + msg_hour  //bom dia, segue o checklist:
        + "\n\n"    //quebra de linha dupla
        + item_name_and_ip;

    copy_clipboard(last_return);

    return last_return;

}

string detect_type(string receiver_zero) {

    regex rgx_unit("Unidade:");
    regex rgx_vmwa("VMWare:");
    string item_type;

    //detectar se é uma unidade ou vmware //fazer função depois.
    if (regex_search(receiver_zero, rgx_unit)) {
        item_type = "Unidade";
    }
    if (regex_search(receiver_zero, rgx_vmwa)) {
        item_type = "VMWare";
    }
    //

    return item_type;

}

int main() {

    string item, first_q, incident_type, full_incidents, backup_last_full_incidents;
    vector<vector<string>> all_receivers;

    bool first_quest = true, second_report = false; //var que se ativa de cara, para a pergunta ser feita de primeira

    while (true) {

        vector<string> items;
        vector<string> receiver;

        regex rgx_comma_items("([^,\\s+]+)"); //regex para coletar tudo que não seja virgula e espaços '^,\\s+'
        smatch smatch_comma_items;

        string item_type, use_report;

        if (first_quest == true) {
            first_quest = false;

            limpar_tela();

            /*
            cout << "\n[1] Analisar Itens\n[2] Analisar e Reportar itens\n[3] Gerar report comum\n[4] Outros Reports\n\n> ";
            first_q = getanswer();

            regex rgx_choices("[" + first_q + "]");
            if (regex_search("12", rgx_choices)) {
                //fazer analise ou fazer analise e reportar depois
            }

            if (first_q == "3") {
                //gerar texto de report comum, BRK, Qualy, Data
            }
            */

            cout << "\nDigite o item a ser analisado:\n\n> ";
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
        }

        int count = 0;
        for (string current_item : items) {

            //limpar_tela();
            //primeiro crio o vetor receiver com os dados do item, depois adiciono ele a ultima linha do vetor de vetores
            vector<string> receiver = detect_item_type_single(current_item);
            all_receivers.push_back(receiver);

            //fiz com que a funcao detect_item_type_single retorne o conjunto da mensagem em '0' do vector e exibir os dados do item na tela de uma vez. para facilitar.
            limpar_tela();
            cout << "\nInfomacoes do item:\n" << endl << receiver[0];

            //neste codigo for, "receiver" sera o ultimo vetor listado em all_receivers.]
            //uso meio que pra deixar o item anterior "guardado" e sempre usar o ultimo para mecher.
            for (vector<string> actual_vector : all_receivers) {
                receiver = actual_vector;
            }

            //detectar se é Unidade ou VMWare
            detect_type(receiver[0]);

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
                    itens_show = itens_show + "\033[33mO" + actual_item + "\033[0m" + " ";
                    already_checked = i;
                }
                i++;
            }
            cout << "\nLista com todos os itens: " << itens_show << endl;

            //mudar a cor do item ja verificado para verde usando 'i' do for.
            items[already_checked] = "\033[32mO" + items[already_checked] + "\033[0m";

            //

            if (second_report == true) {
                use_report = "\n[4] usar report do item anterior.";
            }
            else {
                use_report = "";
            }

            //até aqui o ultimo vector de  all_receivers ja foi preenchido com as informações
            //e também já sabemos qual o tipo do item, VM ou unidade

            cout << "\nO que deseja fazer referente a" << item_type << " " << receiver[1] << " agora?:\n\n";
            cout << "[1] Reiniciar programa\n" << "[2] Encerrar programa\n" << "\033[33m[3] Criar novo report\033[0m" << use_report << "\n\n> ";

            item = getanswer();

            if (item == "1") {
                first_quest = true;
                cin.ignore(); //limpar entrada cin (buffer)
                limpar_tela();
                break;
            }

            if (item == "2") {
                exit(0);
            }

            if (item == "3") {

                second_report = true; //essa var serve para fazer a opção 4 só depois que o primeiro report for realizado.

                cout << "\nQual foi o tipo de incidente? \n" << "[1] Queda\n[2] Consumo de CPU\n[3] Consumo de Memoria\n[4] Consumo de Disco\n[5] Memoria Swap\n[6] Sysload\n[7] Temperatura\n\n> ";
                item = getanswer();

                //vector que ira ser preenchido com as respostas de desempenho, cpu, memoria etc
                //depois iremos criar uma string concaternando as informações no for abaixo
                vector<string> perf_questions = performance_questions(item);

                for (string line : perf_questions) {

                    //mensagens relacionadas a performance, concaternar todos os reports na string.

                    if (!line.empty()) { //se line conter um report, adicioneo a string

                        if (full_incidents.empty()) { //verificar se a string esta vazia, se sim adicionar sem espaçamento.

                            full_incidents = line;
                        }
                        else {
                            full_incidents = full_incidents + " | " + line;

                        }

                    }

                }
                //depois de adicionar o conteudo de identidade do item, iremos adicionar também seus incidentes no vetor de vetores 'all_receivers'
                //sub vetor "count"

                all_receivers[count].push_back(full_incidents);
                final_way_several(all_receivers);

                backup_last_full_incidents = full_incidents; //antes de resetar full_incidents, deixar um backup para usar no 4 (mesmos incidentes)
                full_incidents = ""; //depois de incluir os incidentes na vector, resetar var para novos.

            }

            if (item == "4") {
                limpar_tela();

                //utilizar backup da ultima lista de incidents para repetir no item desejado
                all_receivers[count].push_back(backup_last_full_incidents); //adicionar ao novo report os itens de incident do anterior
                final_way_several(all_receivers);

            }

            count++;
        }

    }

    return 0;
}