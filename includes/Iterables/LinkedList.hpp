#pragma once
#include <stdexcept>
#include <functional>

namespace Iterables {

	template <typename T>
	class LinkedList
	{
	private:
		struct Node {
			T data;
			Node* next;
			Node* previous;
		};
		Node* front;
		Node* back;
		size_t count;
		static const LinkedList<int> emptyLinkList; // static to return faster empty list if requested

		typedef struct Iterator
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = Node;
			using pointer = Node*;  // or also value_type*
			using reference = Node&;  // or also value_type&

			Iterator(pointer ptr) : m_ptr(ptr) {}
			T& operator*() const {
				if (m_ptr == nullptr) throw std::out_of_range("Iterator out of range");
				return m_ptr->data;
			}
			T* operator->() {
				if (m_ptr == nullptr) throw std::out_of_range("Iterator out of range");
				return &m_ptr->data;
			}

			// Prefix increment
			Iterator& operator++() {
				if (m_ptr == nullptr) throw std::out_of_range("Iterator out of range");
				m_ptr = m_ptr->next;
				return *this;
			}
			Iterator& operator+(size_t i) {
				if (m_ptr == nullptr) throw std::out_of_range("Iterator out of range");
				for (size_t j = 0; j < i; j++) {
					if (m_ptr == nullptr) throw std::out_of_range("Iterator out of range");
					m_ptr = m_ptr->next;
				}
				return *this;
			}
			// Postfix increment
			Iterator operator++(int) {
				if (m_ptr == nullptr) throw std::out_of_range("Iterator out of range");
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
			friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
		protected:
			friend class LinkedList;
			pointer m_ptr;
		} iterator;
	public:
		LinkedList() {
			this->front = nullptr;
			this->back = nullptr;
			this->count = 0;
		}
		~LinkedList() {
			Node* current = this->front;
			while (current != nullptr) {
				Node* temp = current;
				current = current->next;
				free(temp);
			}
		}

		LinkedList<T> copy() {
			LinkedList<T> newList;
			Node* current = this->front;
			while (current != nullptr) {
				newList.push_back(current->data);
				current = current->next;
			}
			return newList;
		}

		auto operator =(LinkedList<T> other) {
			this->~LinkedList();
			this->front = other.front;
			this->back = other.back;
			this->count = other.count;
			return *this;
		}

		void insert(T item, size_t pos) {
			if (pos >= this->count) throw std::out_of_range("Index out of range");
			if (pos == this->count - 1) {
				this->back->next = (Node*)malloc(sizeof(Node));
				this->back->next->previous = this->back;
				memcpy(&this->back->next->data, &item, sizeof(T));
				this->back = this->back->next;
			}
			else if (pos == 0) {
				Node* newNode = (Node*)malloc(sizeof(Node));
				memcpy(&newNode->data, &item, sizeof(T));
				newNode->next = this->front;
				newNode->previous = nullptr;
				this->front = newNode;
			}
			else {
				Node* current = this->front;
				for (size_t i = 0; i < pos - 1; i++) {
					current = current->next;
				}
				Node* newNode = (Node*)malloc(sizeof(Node));
				memcpy(&newNode->data, &item, sizeof(T));
				newNode->previous = current;
				newNode->next = current->next;
				current->next->previous = newNode;
				current->next = newNode;
			}
			return;
		}

		iterator begin() { return iterator(front); }

		iterator end() { return iterator(nullptr); }

		void remove(size_t pos) {
			if (pos >= this->count) throw std::out_of_range("Index out of range");
			if (pos == this->count - 1) {
				Node* current = this->front;
				for (size_t i = 0; i < pos - 1; i++) {
					current = current->next;
				}
				free(current->next);
				this->count--;
				current->next = nullptr;
				this->back = current;
			}
			else if (pos == 0) {
				Node* temp = this->front;
				this->front = this->front->next;
				free(temp);
				this->count--;
			}
			else {
				Node* current = this->front;
				for (size_t i = 0; i < pos - 1; i++) {
					current = current->next;
				}
				Node* temp = current->next;
				current->next = temp->next;
				free(temp);
				this->count--;
			}
			return;
		}

		void eraseIf(std::function<bool(T&)> condition) {
			Node* current = this->front;
			while (current != nullptr) {
				if (condition(current->data)) {
					if (current->previous != nullptr) {
						current->previous->next = current->next;
					}
					else {
						this->front = current->next;
					}
					if (current->next != nullptr) {
						current->next->previous = current->previous;
					}
					else {
						this->back = current->previous;
					}
					Node* temp = current;
					current = current->next;
					free(temp);
					this->count--;
				}
				else {
					current = current->next;
				}
			}
		}

		void erase(iterator it) {
			Node* current = this->front;
			while (current != nullptr) {
				if (current == it.m_ptr) {
					if (current->previous != nullptr) {
						current->previous->next = current->next;
					}
					else {
						this->front = current->next;
					}
					if (current->next != nullptr) {
						current->next->previous = current->previous;
					}
					else {
						this->back = current->previous;
					}
					free(current);
					this->count--;
					return;
				}
				current = current->next;
			}
			throw std::out_of_range("Iterator out of range");
		}

		void push_front(T item) {
			Node* newNode = (Node*)malloc(sizeof(Node));
			memcpy(&newNode->data, &item, sizeof(T));
			newNode->next = this->front;
			newNode->previous = nullptr;

			if (this->front != nullptr) {
				this->front->previous = newNode;
			}
			this->front = newNode;
			if (this->back == nullptr) {
				this->back = newNode;
			}
			this->count++;
			return;
		}

		void push_back(T item) {
			Node* newNode = (Node*)malloc(sizeof(Node));
			memcpy(&newNode->data, &item, sizeof(T));
			newNode->next = nullptr;
			newNode->previous = this->back;

			if (this->back != nullptr) {
				this->back->next = newNode;
			}
			this->back = newNode;
			if (this->front == nullptr) {
				this->front = newNode;
			}
			this->count++;
			return;
		}

		T pop_front() {
			T item;
			if (this->front == nullptr) return item;
			item = this->front->data;
			Node* temp = this->front;
			this->front = this->front->next;
			free(temp);
			if (this->front == nullptr) {
				this->back = nullptr;
			}
			this->count--;
			return item;
		}

		T pop_back() {
			T item;
			if (this->back == nullptr) return item;
			if (this->front == this->back) {
				item = this->back->data;
				free(this->back);
				this->front = nullptr;
				this->back = nullptr;
			}
			else {
				Node* current = this->front;
				while (current->next != this->back) {
					current = current->next;
				}
				item = this->back->data;
				free(this->back);
				this->back = current;
				this->back->next = nullptr;
			}
			this->count--;
			return item;
		}

		size_t const size() {
			return this->count;
		}

		T& operator[](int pos) {
			if (pos >= this->count) throw std::out_of_range("Index out of range");
			Node* current = this->front;
			for (size_t i = 0; i < pos; i++) {
				current = current->next;
			}
			return current->data;
		}

		bool is(LinkedList<T>& other) {
			if (this->size() != other.size()) return false;
			Node* current1 = this->front;
			Node* current2 = other.front;
			while (current1 != nullptr) {
				if (current1->data != current2->data) return false;
				current1 = current1->next;
				current2 = current2->next;
			}
			return true;
		}

		template <typename R>
		bool compare(LinkedList<R> other, std::function<bool(T&, R&)> comparator) {
			if (this->size() != other.size()) return false;
			Node* current1 = this->front;
			typename LinkedList<R>::Node* current2 = other.front;
			while (current1 != nullptr) {
				if (!comparator(current1->data, current2->data)) return false;
				current1 = current1->next;
				current2 = current2->next;
			}
			return true;
		}

		constexpr LinkedList<T> Empty() {
			return *((LinkedList<T>*)(&emptyLinkList));
		}
	};
}