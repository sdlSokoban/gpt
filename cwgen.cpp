#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

using namespace std;

// Define a estrutura da grade de palavras cruzadas
struct CrosswordGrid
{
    vector<string> grid;
    vector<pair<int, int>> positions;
    unordered_map<int, string> word_map;
    int size;
};

class CwdGen
{
	public:
		/*Default constructor */
		CwdGen();

		/* Default destructor */
		~CwdGen();

		/* Read word file */
		void readWordFile(const string& file_path);

		/* Generate crossword */
		void generateCrossword();

		/* Print crossword grid */
		void printToFile(const string& file_path) const;

	private:
		/* Private variables */
		vector<string> words;
		vector<string> used_words;
		unique_ptr<CrosswordGrid> grid_ptr;

		/* Private methods */
		void createGrid();
		bool fitWord(const string& word, int row, int col, int direction);
		bool backtrack(int word_index);
		void addWordToGrid(const string& word, int row, int col, int direction);
		void removeWordFromGrid(const string& word);
};

CwdGen::CwdGen()
{
}

CwdGen::~CwdGen()
{
}

void CwdGen::readWordFile(const string& file_path)
{
    /* Open input file */
    ifstream input_file(file_path);
    if (!input_file)
    {
        cerr << "Não foi possível abrir o arquivo de palavras." << endl;
        return;
    }

    /* Read words from file */
    string word;
    while (input_file >> word)
    {
        transform(word.begin(), word.end(), word.begin(), ::toupper);
        words.push_back(word);
    }

    /* Close input file */
    input_file.close();

    /*Sort word list in ascending mode */
    sort(words.begin(), words.end(), [](const string& a, const string& b) { return a.length() > b.length(); });
}

void CwdGen::generateCrossword()
{
	/* Create crossword grid */
	createGrid();

	/* Sort word list */
	sort(words.begin(), words.end(), [](const string& a, const string& b)
	{
		return a.length() > b.length();
	});

	/* Try to fit word in grid */
	for (int i = 0; i < words.size(); i++)
	{
		/* Check if word is used */
		if (find(used_words.begin(), used_words.end(), words[i]) != used_words.end())
		{
			continue;
		}

		/* Try to fit word at grid position */
		for (int j = 0; j < grid_ptr->positions.size(); j++)
		{
			pair<int, int> pos = grid_ptr->positions[j];
			if (fitWord(words[i], pos.first, pos.second, 0) || fitWord(words[i], pos.first, pos.second, 1))
			{
				/* Add word to grid */
				addWordToGrid(words[i], pos.first, pos.second, fitWord(words[i], pos.first, pos.second, 0) ? 0 : 1);
				used_words.push_back(words[i]);

				/* Check if all words is used */
				if (used_words.size() == words.size())
				{
					return;
				}

				/* Try to fit next word */
				if (backtrack(i + 1))
				{
					return;
				}

				/* Remove word from grid */
				removeWordFromGrid(words[i]);
				used_words.pop_back();
			}
		}
	}
}

void CwdGen::printToFile(const string& file_path) const
{
	ofstream output_file(file_path);
	if (output_file.is_open())
	{
		for (int i = 0; i < grid_ptr->size; i++)
		{
			for (int j = 0; j < grid_ptr->size; j++)
			{
				output_file << grid_ptr->grid[i][j] << " ";
			}
			output_file << endl;
		}
		output_file << endl;
		for (auto& entry : grid_ptr->word_map)
		{
			output_file << entry.first << " " << entry.second << endl;
		}
		output_file.close();
	}
	else
	{
		cerr << "Error: Unable to open output file." << endl;
	}
}

void CwdGen::createGrid()
{
	/* Sort word list */
	sort(words.begin(), words.end(), [](const string& a, const string& b){return a.size()>b.size();});

	/* Set grid size */
	//int max_word_size = words.front().size();
	int max_word_size = 6;
	//int grid_size = max_word_size * 2 - 1;
	int grid_size = 15;
	grid_ptr = make_unique<CrosswordGrid>();
	grid_ptr->size = grid_size;

	/* Initialize grid */
	for (int i = 0; i < grid_size; i++)
	{
		string row(grid_size, ' ');
		grid_ptr->grid.push_back(row);
	}

	/* Add initial positions in grid */
	for (int i = 0; i < grid_size; i++)
	{
		for (int j = 0; j < grid_size; j++)
		{
			if (i == max_word_size - 1 || j == max_word_size - 1 || i + j == max_word_size * 2 - 2)
			{
				grid_ptr->positions.emplace_back(i, j);
			}
		}
	}
}

bool CwdGen::fitWord(const string& word, int row, int col, int direction)
{
	//cout << word.c_str() << endl;
    /* Check if word fit */
    if (word.size() > (direction == 0 ? grid_ptr->grid[row].size() - col : grid_ptr->grid.size() - row))
    {
        return false;
    }

    /* Check if word is wrong at position */
    for (int i = 0; i < word.size(); i++)
    {
        int new_row = row + (direction == 1 ? i : 0);
        int new_col = col + (direction == 0 ? i : 0);

        if (grid_ptr->grid[new_row][new_col] != '-' && grid_ptr->grid[new_row][new_col] != word[i])
        {
            return false;
        }
    }

    /* The word is possible to insert in grid */
    return true;
}

bool CwdGen::backtrack(int word_index)
{
	/* Check if all words inserted into grid */
	if (word_index == words.size())
	{
	return true;
	}
	/* Find next empty position in grid */
	auto& positions = grid_ptr->positions;
	int index = find(grid_ptr->grid.begin(), grid_ptr->grid.end(), ".") - grid_ptr->grid.begin();
	int row = index / grid_ptr->size;
	int col = index % grid_ptr->size;

	/* Try to adjust next word at current position */
	for (int i = word_index; i < words.size(); i++)
	{
		/* Check if word is not used */
		if (find(used_words.begin(), used_words.end(), words[i]) == used_words.end())
		{
			/* Try to fit word at actual position in both directions */
			if (fitWord(words[i], row, col, 0) || fitWord(words[i], row, col, 1))
			{
				/* Add word to grid */
				addWordToGrid(words[i], row, col, 0);

				/* Perform a recursive call for a next word */
				if (backtrack(word_index + 1))
				{
					return true;
				}

				/* Remove word from grid */
				removeWordFromGrid(words[i]);
			}
		}
	}

	/* Could not fit any word to current position, so returns false */
	return false;
}

void CwdGen::addWordToGrid(const string& word, int row, int col, int direction)
{
	/* Add word to grid at defined position */
	for (int i = 0; i < word.length(); i++)
	{
		if (direction == 0)
		{
			grid_ptr->grid[row][col + i] = word[i];
			grid_ptr->positions.push_back(make_pair(row, col + i));
		}
		else
		{
			grid_ptr->grid[row + i][col] = word[i];
			grid_ptr->positions.push_back(make_pair(row + i, col));
		}
	}

	/* Add word to words map */
	grid_ptr->word_map.insert(make_pair(used_words.size(), word));

	/* Add word to used words list */
	used_words.push_back(word);
}

void CwdGen::removeWordFromGrid(const string& word)
{
    CrosswordGrid& grid = *grid_ptr;

    /* Remove word from grid */
    for (const auto& position : grid.positions)
    {
        int row = position.first;
        int col = position.second;

        for (int i = 0; i < word.length(); ++i)
        {
            if (grid.grid[row][col] == word[i])
            {
                if (i == 0)
                {
                    /* Check if word is at horizontal or vertical position */
                    int direction = 0;
                    if (row > 0 && grid.grid[row - 1][col] == word[1])
                    {
                        direction = 1;
                    }

                    /* Remove a word from word_map */
                    for (int j = 0; j < word.length(); ++j)
                    {
                        if (direction == 0)
                        {
                            grid.word_map[row * grid.size + col + j] = "";
                        }
                        else
                        {
                            grid.word_map[(row + j) * grid.size + col] = "";
                        }
                    }

                    /* Remove a word from grid */
                    for (int j = 0; j < word.length(); ++j)
                    {
                        if (direction == 0)
                        {
                            grid.grid[row][col + j] = '.';
                        }
                        else
                        {
                            grid.grid[row + j][col] = '.';
                        }
                    }

                    return;
                }
            }
            else
            {
                break;
            }
        }
    }
}

int main(int argc, char* argv[])
{
	// Cria uma instância da classe CrosswordCreator
	CwdGen crossword;

	// Lê o arquivo de palavras "palavras.txt"
	crossword.readWordFile("./palavras.txt");

	// Gera as palavras cruzadas
	crossword.generateCrossword();

	// Imprime a grade gerada no arquivo "palavras_cruzadas.txt"
	crossword.printToFile("./palavras_cruzadas.txt");

	return 0;
}