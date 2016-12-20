// tick-tack-toe.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <memory>
#include <iostream>

class Mass {
public:
	enum status {
		BLANK,
		PLAYER,
		ENEMY,
	};
private:
	status s_ = BLANK;
public:
	void setStatus(status s) { s_ = s; }
	status getStatus() const { return s_; }

	bool put(status s) {
		if (s_ != BLANK) return false;
		s_ = s;
		return true;
	}
};

class Board;

class AI {
public:
	AI() {}
	virtual ~AI() {}

	virtual bool think(Board &b) = 0;

public:
	enum type {
		TYPE_ORDERED = 0,
		TYPE_NEGA_MAX,
	};

	static AI* createAi(type type);
};

// ���Ԃɑł��Ă݂�
class AI_ordered : public AI {
public:
	AI_ordered() {}
	~AI_ordered() {}

	bool think(Board &b);
};

class AI_nega_max : public AI{
private:
	int evaluate(Board &b, Mass::status next, signed int sign);
public:
	AI_nega_max() {}
	~AI_nega_max() {}

	bool think(Board &b);
};

AI* AI::createAi(type type)
{
	switch (type) {
	case TYPE_NEGA_MAX:
		return new AI_nega_max();
		break;
	default: // case TYPE_ORDERED:
		return new AI_ordered();
		break;
	}

	return nullptr;
}

class Board 
{
	friend class AI_ordered;
	friend class AI_nega_max;

public:
	enum {
		NOT_FINISED = 0,
		PLAYER,
		ENEMY,
		DRAW,
	};
private:
	enum {
		BOARD_SIZE = 3,
	};
	Mass mass_[BOARD_SIZE][BOARD_SIZE];

public:
	int calc_result() const
	{
		// ��
		for (int y = 0; y < BOARD_SIZE; y++) {
			Mass::status winner = mass_[y][0].getStatus();
			if (winner != Mass::PLAYER && winner != Mass::ENEMY) continue;
			int x = 1;
			for (; x < BOARD_SIZE; x++) {
				if (mass_[y][x].getStatus() != winner) break;
			}
			if (x == BOARD_SIZE) {
				return winner;
			}
		}
		// �c
		for (int x = 0; x < BOARD_SIZE; x++) {
			Mass::status winner = mass_[0][x].getStatus();
			if (winner != Mass::PLAYER && winner != Mass::ENEMY) continue;
			int y = 1;
			for (; y < BOARD_SIZE; y++) {
				if (mass_[y][x].getStatus() != winner) break;
			}
			if (y == BOARD_SIZE) {
				return winner;
			}
		}
		// �΂�
		{
			Mass::status winner = mass_[0][0].getStatus();
			if (winner == Mass::PLAYER || winner == Mass::ENEMY) {
				int idx = 1;
				for (; idx < BOARD_SIZE; idx++) {
					if (mass_[idx][idx].getStatus() != winner) break;
				}
				if (idx == BOARD_SIZE) {
					return winner;
				}
			}
		}
		{
			Mass::status winner = mass_[BOARD_SIZE - 1][0].getStatus();
			if (winner == Mass::PLAYER || winner == Mass::ENEMY) {
				int idx = 1;
				for (; idx < BOARD_SIZE; idx++) {
					if (mass_[BOARD_SIZE - 1 - idx][idx].getStatus() != winner) break;
				}
				if (idx == BOARD_SIZE) {
					return winner;
				}
			}
		}
		// ��L���s�����Ă��炸�A�c��̃}�X���Ȃ���Έ�����
		for (int y = 0; y < BOARD_SIZE; y++) {
			for (int x = 0; x < BOARD_SIZE; x++) {
				Mass::status fill = mass_[y][x].getStatus();
				if (fill == Mass::BLANK) return NOT_FINISED;
			}
		}
		return DRAW;
	}

	bool put(int x, int y) {
		if (x < 0 || BOARD_SIZE <= x || 
			y < 0 || BOARD_SIZE <= y) return false;// �ՖʊO
		return mass_[y][x].put(Mass::PLAYER);
	}

	void show() const {
		std::cout << "�@�@";
		for (int x = 0; x < BOARD_SIZE; x++) {
			std::cout << " " << x + 1 << "�@";
		}
		std::cout << "\n�@";
		for (int x = 0; x < BOARD_SIZE; x++) {
			std::cout << "�{�|";
		}
		std::cout << "�{\n";
		for (int y = 0; y < BOARD_SIZE; y++) {
			std::cout << " " << char('a' + y);
			for (int x = 0; x < BOARD_SIZE; x++) {
				std::cout << "�b";
				switch (mass_[y][x].getStatus()) {
				case Mass::PLAYER:
					std::cout << "�Z";
					break;
				case Mass::ENEMY:
					std::cout << "�~";
					break;
				case Mass::BLANK:
					std::cout << "�@";
					break;
				default:
					//					if (mass_[y][x].isListed(Mass::CLOSE)) std::cout << "�{"; else
					//					if (mass_[y][x].isListed(Mass::OPEN) ) std::cout << "��"; else
					std::cout << "�@";
				}
			}
			std::cout << "�b\n";
			std::cout << "�@";
			for (int x = 0; x < BOARD_SIZE; x++) {
				std::cout << "�{�|";
			}
			std::cout << "�{\n";
		}
	}
};

bool AI_ordered::think(Board &b)
{
	for (int y = 0; y < Board::BOARD_SIZE; y++) {
		for (int x = 0; x < Board::BOARD_SIZE; x++) {
			if (b.mass_[y][x].put(Mass::ENEMY)) {
				return true;
			}
		}
	}
	return false;
}


// int depth = 0;
int AI_nega_max::evaluate(Board &b, Mass::status current, signed int sign)
{
	Mass::status next = (current == Mass::ENEMY) ? Mass::PLAYER : Mass::ENEMY;
	// ��������
	int r = b.calc_result();
	if (r == Mass::ENEMY) return +10000;// AI�̏���
	if (r == Mass::PLAYER) return -10000;// AI�̕���
	if (r == Board::DRAW) return 0;// ������

	int score_max = -10001;
//	depth++;
//

	for (int y = 0; y < Board::BOARD_SIZE; y++) {
		for (int x = 0; x < Board::BOARD_SIZE; x++) {
			Mass &m = b.mass_[y][x];
			if (m.getStatus() != Mass::BLANK) continue;

			m.setStatus(next);// ���̎��ł�
			int score = sign * evaluate(b, next, -sign);
			m.setStatus(Mass::BLANK);// ���߂�

//
//			for (int i = 0; i < depth; i++) {
//				std::cout << " ";
//			}
//			std::cout << x + 1 << (char)('a' + y) << " " << sign * score << std::endl;
//
			score_max = (score_max < score) ? score : score_max;// �ŗǂ̌��ʂ�ۑ�
		}
	}
//	depth--;
//

	return sign * score_max;
}

bool AI_nega_max::think(Board &b)
{
	int best_x = -1, best_y;
	int score_max = -10001;

	for (int y = 0; y < Board::BOARD_SIZE; y++) {
		for (int x = 0; x < Board::BOARD_SIZE; x++) {

			Mass &m = b.mass_[y][x];
			if (m.getStatus() != Mass::BLANK) continue;// �łĂȂ��}�X

			m.setStatus(Mass::ENEMY);// ���ł�

			int score = evaluate(b, Mass::ENEMY, -1);
//
//			std::cout << "* " << x+1 << (char)('a' + y) << " " << score << std::endl;
//
			m.setStatus(Mass::BLANK);// ���߂�

			if (score_max < score) {
				score_max = score;
				best_x = x;
				best_y = y;
			}
		}
	}
	if (best_x < 0) return false; // ������Ȃ�����

	return b.mass_[best_y][best_x].put(Mass::ENEMY);
}

class Game
{
private:
	const AI::type ai_type = AI::TYPE_NEGA_MAX;

	Board board_;
	int winner_ = Board::NOT_FINISED;
	AI *pAI_ = nullptr;

public:
	Game() {
		pAI_ = AI::createAi(ai_type);
	}
	~Game() {
		delete pAI_;
	}

	bool put(int x, int y) {
		bool success = board_.put(x, y);
		if (success) winner_ = board_.calc_result();
		
		return success;
	}

	bool think() {
		bool success = pAI_->think(board_);
		if (success) winner_ = board_.calc_result();
		return success;
	}

	int is_finised() {
		return winner_;
	}

	void show() {
		board_.show();
	}
};




void show_start_message()
{
	std::cout << "========================" << std::endl;
	std::cout << "       GAME START       " << std::endl;
	std::cout << std::endl;
	std::cout << "input position likes 1 a" << std::endl;
	std::cout << "========================" << std::endl;
}

void show_end_message(int winner)
{
	if (winner == Board::PLAYER) {
		std::cout << "You win!" << std::endl;
	}
	else if (winner == Board::ENEMY)
	{
		std::cout << "You lose..." << std::endl;
	}
	else {
		std::cout << "Draw" << std::endl;
	}
	std::cout << std::endl;
}

int main()
{
	for (;;) {// �������[�v
		show_start_message();

		// initialize
		unsigned int turn = 0;
		std::shared_ptr<Game> game(new Game());

		while (1) {
			game->show();// �Ֆʕ\��

			// ��������
			int winner = game->is_finised();
			if (winner) {
				show_end_message(winner);
				break;

			}

			if (0 == turn) {
				// user input
				char col[1], row[1];
				do {
					std::cout << "? ";
					std::cin >> row >> col;
				} while (!game->put(row[0] - '1', col[0] - 'a'));
			}
			else {
				// AI
				game->think();
				std::cout << std::endl;
			}
			// �v���C���[��AI�̐؂�ւ�
			turn = 1 - turn;
		}
	}

	return 0;
}

