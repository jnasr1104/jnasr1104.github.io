from pymongo import MongoClient, errors
from bson.objectid import ObjectId

class AnimalShelter:
    """
    Provides CRUD operations for the AAC MongoDB collection.
    Designed for integration with a local MongoDB instance by default.
    """

    def __init__(self, username: str, password: str, host: str = 'localhost', port: int = 27017, db: str = 'AAC', col: str = 'animals'):
        """
        Initializes a connection to the MongoDB database using provided credentials.
        Default connection targets a local MongoDB instance.
        """
        try:
            self.client = MongoClient(f'mongodb://{username}:{password}@{host}:{port}')
            self.database = self.client[db]
            self.collection = self.database[col]
        except errors.ConnectionFailure as e:
            raise Exception(f"Could not connect to MongoDB: {e}")

    def create(self, data: dict) -> bool:
        """
        Inserts a new animal document into the MongoDB collection.

        Args:
            data (dict): The document to insert. Must be a valid dictionary.

        Returns:
            bool: True if the document was inserted successfully, False otherwise.
        """
        if not data or not isinstance(data, dict):
            raise ValueError("Data must be a non-empty dictionary.")
        try:
            result = self.collection.insert_one(data)
            return result.inserted_id is not None
        except Exception as e:
            print(f"Create operation failed: {e}")
            return False

    def read(self, criteria: dict = None):
        """
        Reads animal records from the collection based on optional criteria.
        Omits the MongoDB-generated '_id' field from the results for UI compatibility.

        Args:
            criteria (dict, optional): MongoDB query filter. Returns all records if None.

        Returns:
            Cursor or list: Matching records.
        """
        try:
            query = criteria if criteria else {}
            return self.collection.find(query, {"_id": False})
        except Exception as e:
            print(f"Read operation failed: {e}")
            return []

    def update(self, initial: dict, change: dict):
        """
        Updates documents in the collection that match the initial filter with new values.

        Args:
            initial (dict): The filter to match existing documents.
            change (dict): Fields and values to update.

        Returns:
            dict: Result metadata including counts or errors.
        """
        if not initial or not isinstance(initial, dict):
            raise ValueError("Initial match criteria must be a non-empty dictionary.")
        if not change or not isinstance(change, dict):
            raise ValueError("Update changes must be a non-empty dictionary.")

        try:
            if self.collection.count_documents(initial, limit=1):
                update_result = self.collection.update_many(initial, {"$set": change})
                return update_result.raw_result
            else:
                return {"matched": 0, "modified": 0, "message": "No documents matched the criteria."}
        except Exception as e:
            print(f"Update operation failed: {e}")
            return {"error": str(e)}

    def delete(self, remove: dict):
        """
        Deletes documents in the collection that match the given filter.

        Args:
            remove (dict): Filter to select documents to delete.

        Returns:
            dict: Result metadata including count of deletions or errors.
        """
        if not remove or not isinstance(remove, dict):
            raise ValueError("Remove criteria must be a non-empty dictionary.")

        try:
            if self.collection.count_documents(remove, limit=1):
                delete_result = self.collection.delete_many(remove)
                return delete_result.raw_result
            else:
                return {"deleted": 0, "message": "No documents matched the criteria."}
        except Exception as e:
            print(f"Delete operation failed: {e}")
            return {"error": str(e)}
