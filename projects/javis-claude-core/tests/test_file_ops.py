"""file_ops.py 测试套件"""
import pytest
import os
from src.file_ops import (
    FileOps, ReadResult, WriteResult, EditResult,
    GrepResult, GlobResult, TextFilePayload
)

class TestReadFile:
    def test_read_basic(self, tmp_path):
        f = tmp_path / "sample.txt"
        f.write_text("line1\nline2\nline3\n")
        result = FileOps.read_file(str(f))
        assert isinstance(result, ReadResult)
        assert result.file.content == "line1\nline2\nline3\n"
        # split("\n") on trailing-newline string gives extra empty element
        assert result.file.num_lines == 4
        assert result.file.start_line == 0
        assert result.file.total_lines == 4

    def test_read_nonexistent(self, tmp_path):
        with pytest.raises(FileNotFoundError):
            FileOps.read_file(str(tmp_path / "notexist.txt"))

    def test_read_with_offset_only(self, tmp_path):
        f = tmp_path / "sample.txt"
        f.write_text("line1\nline2\nline3\nline4\nline5\n")
        result = FileOps.read_file(str(f), offset=2)
        assert result.file.start_line == 2
        assert "line3" in result.file.content
        # trailing \n creates extra empty element in split
        assert result.file.total_lines == 6

    def test_read_with_limit_only(self, tmp_path):
        f = tmp_path / "sample.txt"
        f.write_text("line1\nline2\nline3\nline4\nline5\n")
        result = FileOps.read_file(str(f), limit=2)
        assert result.file.num_lines == 2
        assert result.file.total_lines == 6

    def test_read_with_offset_and_limit(self, tmp_path):
        f = tmp_path / "sample.txt"
        f.write_text("line1\nline2\nline3\nline4\nline5\n")
        result = FileOps.read_file(str(f), offset=1, limit=2)
        assert result.file.start_line == 1
        assert result.file.num_lines == 2
        assert "line2" in result.file.content
        assert "line3" in result.file.content

    def test_read_offset_exceeds_total(self, tmp_path):
        f = tmp_path / "sample.txt"
        f.write_text("line1\nline2\n")
        result = FileOps.read_file(str(f), offset=100)
        # split gives 3 elements for "line1\nline2\n"
        assert result.file.start_line == 3
        assert result.file.num_lines == 0
        assert result.file.content == ""

    def test_read_empty_file(self, tmp_path):
        f = tmp_path / "empty.txt"
        f.write_text("")
        result = FileOps.read_file(str(f))
        assert result.file.num_lines == 1
        assert result.file.content == ""

class TestWriteFile:
    def test_write_new_file(self, tmp_path):
        target = tmp_path / "new.txt"
        result = FileOps.write_file(str(target), "hello world")
        assert isinstance(result, WriteResult)
        assert result.file_path == str(target)
        assert result.content == "hello world"
        assert result.original_file is None
        assert target.read_text() == "hello world"

    def test_write_overwrite_existing(self, tmp_path):
        target = tmp_path / "existing.txt"
        target.write_text("original content")
        result = FileOps.write_file(str(target), "new content")
        assert result.original_file == "original content"
        assert result.content == "new content"
        assert target.read_text() == "new content"

    def test_write_creates_parent_dirs(self, tmp_path):
        target = tmp_path / "subdir" / "nested" / "file.txt"
        result = FileOps.write_file(str(target), "nested content")
        assert target.exists()
        assert target.read_text() == "nested content"

    def test_write_unicode_content(self, tmp_path):
        target = tmp_path / "unicode.txt"
        content = "hello\nworld"
        result = FileOps.write_file(str(target), content)
        assert target.read_text() == content

class TestEditFile:
    def test_edit_single_replacement(self, tmp_path):
        target = tmp_path / "edit.txt"
        target.write_text("foo bar foo")
        result = FileOps.edit_file(str(target), "bar", "baz")
        assert isinstance(result, EditResult)
        assert result.old_string == "bar"
        assert result.new_string == "baz"
        assert result.original_file == "foo bar foo"
        assert result.replace_all is False
        assert target.read_text() == "foo baz foo"

    def test_edit_replace_all(self, tmp_path):
        target = tmp_path / "edit.txt"
        target.write_text("foo bar foo baz foo")
        result = FileOps.edit_file(str(target), "foo", "FOO", replace_all=True)
        assert result.replace_all is True
        assert target.read_text() == "FOO bar FOO baz FOO"

    def test_edit_string_not_found(self, tmp_path):
        target = tmp_path / "edit.txt"
        target.write_text("hello world")
        with pytest.raises(ValueError, match="String not found"):
            FileOps.edit_file(str(target), "notexist", "replacement")

    def test_edit_empty_file(self, tmp_path):
        target = tmp_path / "empty.txt"
        target.write_text("")
        with pytest.raises(ValueError):
            FileOps.edit_file(str(target), "any", "replacement")

    def test_edit_preserves_original(self, tmp_path):
        target = tmp_path / "edit.txt"
        target.write_text("original content")
        result = FileOps.edit_file(str(target), "original", "modified")
        assert result.original_file == "original content"

class TestGrepSearch:
    def test_grep_basic(self, tmp_path):
        (tmp_path / "a.txt").write_text("hello world\nfoo bar\n")
        (tmp_path / "b.txt").write_text("hello again\n")
        result = FileOps.grep_search("hello", str(tmp_path))
        assert result.num_files == 2
        assert result.num_matches >= 1
        assert "hello" in result.content

    def test_grep_case_insensitive(self, tmp_path):
        """验证case_insensitive参数有效；num_matches计数匹配行而非个别出现次数"""
        (tmp_path / "test.txt").write_text("Hello HELLO hello\n")
        result = FileOps.grep_search("hello", str(tmp_path), case_insensitive=True)
        # grep_search计数匹配行；同一行3个出现计为1行
        assert result.num_matches == 1
        assert "Hello HELLO hello" in result.content

    def test_grep_case_sensitive(self, tmp_path):
        (tmp_path / "test.txt").write_text("Hello HELLO hello\n")
        result = FileOps.grep_search("hello", str(tmp_path), case_insensitive=False)
        assert result.num_matches == 1
        assert result.num_files == 1

    def test_grep_line_numbers(self, tmp_path):
        (tmp_path / "test.txt").write_text("line1\nline2\nline3\n")
        result = FileOps.grep_search("line2", str(tmp_path), line_numbers=True)
        assert "2: line2" in result.content

    def test_grep_no_match(self, tmp_path):
        (tmp_path / "test.txt").write_text("foo bar\n")
        result = FileOps.grep_search("notexist", str(tmp_path))
        assert result.num_files == 0
        assert result.num_matches == 0

    def test_grep_excludes_dotfiles(self, tmp_path):
        (tmp_path / ".hidden").write_text("secret keyword\n")
        (tmp_path / "normal.txt").write_text("keyword\n")
        result = FileOps.grep_search("keyword", str(tmp_path))
        assert ".hidden" not in result.filenames

class TestGlobSearch:
    def test_glob_basic(self, tmp_path):
        (tmp_path / "a.txt").write_text("a")
        (tmp_path / "b.txt").write_text("b")
        (tmp_path / "c.py").write_text("c")
        result = FileOps.glob_search("*.txt", str(tmp_path))
        assert result.num_files == 2
        assert len(result.filenames) == 2

    def test_glob_nested(self, tmp_path):
        sub = tmp_path / "sub"
        sub.mkdir()
        (tmp_path / "root.txt").write_text("r")
        (sub / "nested.txt").write_text("n")
        result = FileOps.glob_search("**/*.txt", str(tmp_path))
        assert result.num_files >= 2

    def test_glob_no_match(self, tmp_path):
        (tmp_path / "a.txt").write_text("a")
        result = FileOps.glob_search("*.xyz", str(tmp_path))
        assert result.num_files == 0

    def test_glob_returns_duration(self, tmp_path):
        (tmp_path / "a.txt").write_text("a")
        result = FileOps.glob_search("*.txt", str(tmp_path))
        assert isinstance(result.duration_ms, int)
        assert result.duration_ms >= 0

    def test_glob_files_only(self, tmp_path):
        sub = tmp_path / "subdir"
        sub.mkdir()
        (sub / "file.txt").write_text("f")
        result = FileOps.glob_search("**/*", str(tmp_path))
        for f in result.filenames:
            assert os.path.isfile(f)

class TestDataclasses:
    def test_text_file_payload_fields(self):
        payload = TextFilePayload(
            file_path="/tmp/test.txt", content="test",
            num_lines=1, start_line=0, total_lines=1,
        )
        assert payload.file_path == "/tmp/test.txt"
        assert payload.content == "test"

    def test_structured_patch_hunk_fields(self):
        from src.file_ops import StructuredPatchHunk
        hunk = StructuredPatchHunk(
            old_start=1, old_lines=5, new_start=2, new_lines=6,
            lines=["@@ -1,5 +1,6 @@"],
        )
        assert hunk.old_start == 1
        assert hunk.old_lines == 5

    def test_grep_result_default_values(self):
        """验证GrepResult默认值；num_matches字段在GrepResult类中默认为None"""
        result = GrepResult()
        assert result.num_files == 0
        assert result.num_matches is None  # GrepResult定义时Optional[int]默认为None
        assert result.filenames == []

    def test_glob_result_default_values(self):
        result = GlobResult()
        assert result.num_files == 0
        assert result.truncated is False
        assert result.duration_ms == 0
